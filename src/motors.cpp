/**
 * \file devices/vdml_motors.cpp
 *
 * Contains functions for interacting with the V5 Motors.
 *
 * Copyright (c) 2017-2020, Purdue University ACM SIGBots.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "./ncrLvglSimKernel.hpp"
#if USE_PROS_LVGL_SIM == 1
#include "pros/motors.hpp"
#include "pros/rtos.hpp"
#include <iostream>
#include <vector>

#define MOTOR_MOVE_RANGE 127
#define MOTOR_VOLTAGE_RANGE 12000
class MotorData
{
  public:
    MotorData(int port) : _port(port) {}
    ~MotorData() {}
    double _encNow = 0.0;
    double _speedNow = 0.0;
    int _voltage = 0;
    int _port = 0;
    pros::motor_gearset_e_t _gear = pros::E_MOTOR_GEARSET_18;
    int _reversed = 1;
    double _temperature = 20.0;
    int _isOverTemp = 0;
    pros::motor_encoder_units_e_t _encoder_units = pros::E_MOTOR_ENCODER_DEGREES; //马达编码器的返回值单位
};

std::vector<std::shared_ptr<MotorData>> motorDataList;
void ncrapi::NcrLvglSimKernel::motorSimLoop(const int tickTime)
{
    for (auto it : motorDataList)
    {
        if (it->_gear == pros::E_MOTOR_GEARSET_06) //100转/S
        {
            it->_speedNow = it->_voltage * it->_reversed * tickTime * 0.000833;
        }
        else if (it->_gear == pros::E_MOTOR_GEARSET_18) //200转/S
        {
            it->_speedNow = it->_voltage * it->_reversed * tickTime * 0.001667;
        }
        else //600转/S
        {
            it->_speedNow = it->_voltage * it->_reversed * tickTime * 0.005;
        }
        //  double angularVel = (it->_speedNow * 3.1415926 * it->_reversed) / 30.0;  //角速度 弧度每秒 RPM=angularVel*60/2PI;
        it->_encNow += (3.0 / 500.0 * it->_speedNow * tickTime * it->_reversed); //角度=角速度*时间
        // if (it->_encoder_units == pros::E_MOTOR_ENCODER_DEGREES) //角度累加 默认值
        // {
        //     if (it->_encNow > 360.0)
        //         it->_encNow -= 360.0;
        //     else if (it->_encNow < 0)
        //         it->_encNow += 360.0;
        // }
        // else if (it->_encoder_units == pros::E_MOTOR_ENCODER_ROTATIONS) //弧度
        //     it->_encNow = it->_encNow * (3.1415926 / 180.0);
        // std::cout << "tickTime:" << tickTime << " port:" << it->_port << " vol:" << it->_voltage << " reversed:" << it->_reversed
        //           << " angularVel:" << angularVel << " spd:" << it->_speedNow << " _enc:" << it->_encNow << " encoder_units:" << it->_encoder_units << std::endl;
        //温度模拟
        if (abs(it->_voltage) > 10000)
            it->_temperature += 0.01;
        else
            it->_temperature -= 0.001;
        it->_temperature = std::clamp(it->_temperature, 0.0, 100.0);
        if (it->_temperature > 60)
        {
            it->_isOverTemp = 1;
            it->_voltage /= 60;
        }
        else
            it->_isOverTemp = 0;
    }
}
namespace pros {
using namespace pros::c;

Motor::Motor(const std::uint8_t port, const motor_gearset_e_t gearset, const bool reverse,
             const motor_encoder_units_e_t encoder_units)
    : _port(port)
{
    motorDataList.push_back(std::make_shared<MotorData>(_port));
    set_gearing(gearset);
    set_reversed(reverse);
    set_encoder_units(encoder_units);
}

Motor::Motor(const std::uint8_t port, const motor_gearset_e_t gearset, const bool reverse) : _port(port)
{
    motorDataList.push_back(std::make_shared<MotorData>(_port));
    set_gearing(gearset);
    set_reversed(reverse);
}

Motor::Motor(const std::uint8_t port, const motor_gearset_e_t gearset) : _port(port)
{
    motorDataList.push_back(std::make_shared<MotorData>(_port));
    set_gearing(gearset);
}

Motor::Motor(const std::uint8_t port, const bool reverse) : _port(port)
{
    motorDataList.push_back(std::make_shared<MotorData>(_port));
    set_reversed(reverse);
}

Motor::Motor(const std::uint8_t port) : _port(port) { motorDataList.push_back(std::make_shared<MotorData>(_port)); }

std::int32_t Motor::operator=(std::int32_t voltage) const
{
    voltage = std::clamp<int>(voltage, -127, 127);
    int32_t command = (((voltage + MOTOR_MOVE_RANGE) * (MOTOR_VOLTAGE_RANGE)) / (MOTOR_MOVE_RANGE));
    command -= MOTOR_VOLTAGE_RANGE;
    return move_voltage(command);
}

std::int32_t Motor::move(std::int32_t voltage) const
{
    voltage = std::clamp<int>(voltage, -127, 127);
    int32_t command = (((voltage + MOTOR_MOVE_RANGE) * (MOTOR_VOLTAGE_RANGE)) / (MOTOR_MOVE_RANGE));
    command -= MOTOR_VOLTAGE_RANGE;
    return move_voltage(command);
}

std::int32_t Motor::move_absolute(const double position, const std::int32_t velocity) const
{
    return 1;
}

std::int32_t Motor::move_relative(const double position, const std::int32_t velocity) const
{
    return 1;
}

std::int32_t Motor::move_velocity(const std::int32_t velocity) const
{
    return 1;
}

std::int32_t Motor::move_voltage(const std::int32_t voltage) const
{
    std::int32_t temp = PROS_ERR;
    int voltageTemp = std::clamp<std::int32_t>(voltage, -12000, 12000);
    for (auto it : motorDataList)
        if (it->_port == _port)
        {
            it->_voltage = voltageTemp;
            temp = 1;
        }
    return temp;
}

std::int32_t Motor::modify_profiled_velocity(const std::int32_t velocity) const
{
    return 1;
}

double Motor::get_actual_velocity(void) const
{
    double temp = PROS_ERR_F;
    for (auto it : motorDataList)
        if (it->_port == _port)
            temp = it->_speedNow;
    return temp;
}

motor_brake_mode_e_t Motor::get_brake_mode(void) const
{
    return E_MOTOR_BRAKE_COAST;
}

std::int32_t Motor::get_current_draw(void) const
{
    return 1;
}

std::int32_t Motor::get_current_limit(void) const
{
    return 1;
}

std::int32_t Motor::is_over_current(void) const
{
    return 1;
}

std::int32_t Motor::get_direction(void) const
{
    return 1;
}

double Motor::get_efficiency(void) const
{
    return 1;
}

motor_encoder_units_e_t Motor::get_encoder_units(void) const
{
    motor_encoder_units_e_t temp = E_MOTOR_ENCODER_INVALID;
    for (auto it : motorDataList)
        if (it->_port == _port)
            temp = it->_encoder_units;
    return temp;
}

std::uint32_t Motor::get_faults(void) const
{
    return 1;
}

std::uint32_t Motor::get_flags(void) const
{
    return 1;
}

motor_gearset_e_t Motor::get_gearing(void) const
{
    motor_gearset_e_t temp = E_MOTOR_GEARSET_INVALID;
    for (auto it : motorDataList)
        if (it->_port == _port)
            temp = it->_gear;
    return temp;
}

motor_pid_full_s_t Motor::get_pos_pid(void) const
{
    return {1, 2, 3, 4, 5, 6, 7, 8};
}

motor_pid_full_s_t Motor::get_vel_pid(void) const
{
    return {1, 2, 3, 4, 5, 6, 7, 8};
}

std::int32_t Motor::get_raw_position(std::uint32_t *const timestamp) const
{
    return 1;
}

std::int32_t Motor::is_over_temp(void) const
{
    int temp = PROS_ERR;
    for (auto it : motorDataList)
        if (it->_port == _port)
            temp = it->_isOverTemp;
    return temp;
}

std::int32_t Motor::is_stopped(void) const
{
    return 1;
}

std::int32_t Motor::get_zero_position_flag(void) const
{
    return 1;
}

double Motor::get_position(void) const
{
    double temp = PROS_ERR_F;
    for (auto it : motorDataList)
        if (it->_port == _port)
            temp = it->_encNow;
    return temp;
}

double Motor::get_power(void) const
{
    return 1;
}

std::int32_t Motor::is_reversed(void) const
{
    int temp = PROS_ERR;
    for (auto it : motorDataList)
        if (it->_port == _port)
        {
            if (it->_reversed)
                temp = 1;
            else
                temp = 0;
        }
    return temp;
}

double Motor::get_temperature(void) const
{
    double temp = PROS_ERR_F;
    for (auto it : motorDataList)
        if (it->_port == _port)
            temp = it->_temperature;
    return temp;
}

double Motor::get_target_position(void) const
{
    return 1;
}

double Motor::get_torque(void) const
{
    return 1;
}

std::int32_t Motor::get_target_velocity(void) const
{
    return 1;
}

std::int32_t Motor::get_voltage(void) const
{
    int temp = PROS_ERR;
    for (auto it : motorDataList)
        if (it->_port == _port)
            temp = it->_voltage;
    return temp;
}

std::int32_t Motor::get_voltage_limit(void) const
{
    return 1;
}

std::uint8_t Motor::get_port(void) const
{
    return _port;
}

std::int32_t Motor::tare_position(void) const
{
    return 1;
}

std::int32_t Motor::set_brake_mode(const motor_brake_mode_e_t mode) const
{
    return 1;
}

std::int32_t Motor::set_current_limit(const std::int32_t limit) const
{
    return 1;
}

std::int32_t Motor::set_encoder_units(const motor_encoder_units_e_t units) const
{
    int temp = PROS_ERR;
    for (auto it : motorDataList)
        if (it->_port == _port)
        {
            it->_encoder_units = units;
            temp = 1;
        }
    return temp;
}

std::int32_t Motor::set_gearing(const motor_gearset_e_t gearset) const
{
    int temp = PROS_ERR;
    for (auto it : motorDataList)
        if (it->_port == _port)
        {
            it->_gear = gearset;
            temp = 1;
        }
    return temp;
}

motor_pid_s_t Motor::convert_pid(double kf, double kp, double ki, double kd)
{
    return {1, 2, 3, 4};
}

motor_pid_full_s_t Motor::convert_pid_full(double kf, double kp, double ki, double kd, double filter, double limit,
                                           double threshold, double loopspeed)
{
    return {1, 2, 3, 4, 5, 6, 7, 8};
}

std::int32_t Motor::set_pos_pid(const motor_pid_s_t pid) const
{
    return 1;
}

std::int32_t Motor::set_pos_pid_full(const motor_pid_full_s_t pid) const
{
    return 1;
}

std::int32_t Motor::set_vel_pid(const motor_pid_s_t pid) const
{
    return 1;
}

std::int32_t Motor::set_vel_pid_full(const motor_pid_full_s_t pid) const
{
    return 1;
}

std::int32_t Motor::set_zero_position(const double position) const
{
    return 1;
}

std::int32_t Motor::set_reversed(const bool reverse) const
{
    int temp = PROS_ERR;
    for (auto it : motorDataList)
        if (it->_port == _port)
        {
            if (reverse)
                it->_reversed = 1;
            else
                it->_reversed = -1;
            temp = 1;
        }
    return temp;
}

std::int32_t Motor::set_voltage_limit(const std::int32_t limit) const
{
    return 1;
}

namespace literals {
const pros::Motor operator"" _mtr(const unsigned long long int m)
{
    return pros::Motor(m, false);
}
const pros::Motor operator"" _rmtr(const unsigned long long int m)
{
    return pros::Motor(m, true);
}
} // namespace literals
} // namespace pros
#else
#endif
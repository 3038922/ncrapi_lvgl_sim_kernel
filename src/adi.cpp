/**
 * \file devices/vdml_adi.cpp
 *
 * Contains functions for interacting with the V5 ADI.
 *
 * Copyright (c) 2017-2020, Purdue University ACM SIGBots.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "./ncrLvglSimKernel.hpp"
#if USE_PROS_LVGL_SIM == 1
#include "pros/adi.hpp"

namespace pros {
using namespace pros::c;

ADIPort::ADIPort(std::uint8_t adi_port, adi_port_config_e_t type) : _adi_port(adi_port)
{
}

ADIPort::ADIPort(ext_adi_port_pair_t port_pair, adi_port_config_e_t type)
{
    // for use by derived classes like ADIEncoder
}
std::int32_t ADIPort::get_config(void) const
{
    return _adi_port;
}
std::int32_t ADIPort::get_value(void) const
{
    return 1;
}
std::int32_t ADIPort::set_config(adi_port_config_e_t type) const
{
    return 255;
}

std::int32_t ADIPort::set_value(std::int32_t value) const
{
    return 255;
}

ADIAnalogIn::ADIAnalogIn(std::uint8_t adi_port) : ADIPort(adi_port, E_ADI_ANALOG_IN) {}
ADIAnalogIn::ADIAnalogIn(ext_adi_port_pair_t port_pair) : ADIPort(port_pair, E_ADI_ANALOG_IN)
{
}
std::int32_t ADIAnalogIn::calibrate(void) const
{
    return 1;
}

std::int32_t ADIAnalogIn::get_value_calibrated(void) const
{
    return 1;
}

std::int32_t ADIAnalogIn::get_value_calibrated_HR(void) const
{
    return 1;
}
ADIAnalogOut::ADIAnalogOut(std::uint8_t adi_port) : ADIPort(adi_port, E_ADI_ANALOG_OUT) {}
ADIAnalogOut::ADIAnalogOut(ext_adi_port_pair_t port_pair) : ADIPort(port_pair, E_ADI_ANALOG_OUT) {}

ADIDigitalOut::ADIDigitalOut(std::uint8_t adi_port, bool init_state) : ADIPort(adi_port, E_ADI_DIGITAL_OUT)
{
    set_value(init_state);
}
ADIDigitalOut::ADIDigitalOut(ext_adi_port_pair_t port_pair, bool init_state) : ADIPort(port_pair, E_ADI_DIGITAL_OUT) {}

ADIDigitalIn::ADIDigitalIn(std::uint8_t adi_port) : ADIPort(adi_port, E_ADI_DIGITAL_IN) {}
ADIDigitalIn::ADIDigitalIn(ext_adi_port_pair_t port_pair) : ADIPort(port_pair, E_ADI_DIGITAL_IN) {}

std::int32_t ADIDigitalIn::get_new_press(void) const
{
    return 1;
}

ADIMotor::ADIMotor(std::uint8_t adi_port) : ADIPort(adi_port, E_ADI_LEGACY_PWM)
{
    stop();
}
ADIMotor::ADIMotor(ext_adi_port_pair_t port_pair) : ADIPort(port_pair, E_ADI_LEGACY_PWM) {}

std::int32_t ADIMotor::stop(void) const
{
    return 1;
}

ADIEncoder::ADIEncoder(std::uint8_t adi_port_top, std::uint8_t adi_port_bottom, bool reversed) : ADIPort(adi_port_top, E_ADI_DIGITAL_IN)
{
    _adi_port = adi_port_top;
}
ADIEncoder::ADIEncoder(ext_adi_port_tuple_t port_tuple, bool reversed) : ADIPort(1, E_ADI_DIGITAL_IN)
{
}
std::int32_t ADIEncoder::reset(void) const
{
    return 1;
}

std::int32_t ADIEncoder::get_value(void) const
{
    return 1;
}

ADIUltrasonic::ADIUltrasonic(std::uint8_t adi_port_ping, std::uint8_t adi_port_echo) : ADIPort(adi_port_ping, E_ADI_DIGITAL_IN)
{
    _adi_port = adi_port_ping;
}
ADIUltrasonic::ADIUltrasonic(ext_adi_port_tuple_t port_tuple) : ADIPort(2, E_ADI_DIGITAL_IN) {}

ADIGyro::ADIGyro(std::uint8_t adi_port, double multiplier) : ADIPort(adi_port, E_ADI_DIGITAL_IN)
{
}
ADIGyro::ADIGyro(ext_adi_port_pair_t port_pair, double multiplier) : ADIPort(1, E_ADI_DIGITAL_IN) {}

double ADIGyro::get_value(void) const
{
    return 1;
}

std::int32_t ADIGyro::reset(void) const
{
    return 1;
}
} // namespace pros
#else
#endif
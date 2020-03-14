#include "./ncrLvglSimKernel.hpp"
#if USE_PROS_LVGL_SIM == 1
#include "keyBoardDrive.hpp"
#include "pros/misc.h"
#include <iostream>
extern "C" {
#include "../freeRTOS/src/main.h"
}
namespace ncrapi {
KeyBoard *KeyBoard::_KeyBoard = nullptr; // 单例定义
KeyBoard *KeyBoard::initKeyBoard()
{
    if (_KeyBoard == nullptr)
        _KeyBoard = new KeyBoard();
    return _KeyBoard;
}
KeyBoard::KeyBoard()
{
    lv_indev_drv_init(&_real_kb_drv);
    _real_kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    _real_kb_drv.read_cb = keyboard_read;
    xTaskCreate(KeyBoard::KeyBoardTask, "keyBoardTask", 512, nullptr, TASK_PRIORITY_MAX - 3, nullptr);
}

bool KeyBoard::isPressed()
{
    if (_kbDate.state == LV_INDEV_STATE_PR)
        return true;
    else
    {
        _kbDate.key = KEYBORAD_NO_PR;
        return false;
    }
}
bool KeyBoard::getFiledDigital(const KEYBOARD_DIGITAL btn)
{
    bool flag = false;
    if (isPressed())
        if (_kbDate.key == btn)
            return true;
    return flag;
}
bool KeyBoard::getFiledNewPressed(const KEYBOARD_DIGITAL btn)
{
    bool flag = false;
    if (getFiledDigital(btn))
        while (1)
        {
            if (!getFiledDigital(btn))
            {
                flag = true;
                break;
            }
        }
    return flag;
}
int KeyBoard::getKeyBoardAnalog(const KEYBOARD_ANALOG ch)
{
    return std::clamp(_btnVal[ch], -127, 127);
}
bool KeyBoard::getKeyBoradDigtal(const KEYBOARD_DIGITAL btn)
{
    return _btnVal[btn - 2];
}
bool KeyBoard::getNewPressed(const KEYBOARD_DIGITAL btn)
{
    bool flag = false;
    if (getKeyBoradDigtal(btn))
        while (1)
        {
            if (!getKeyBoradDigtal(btn))
            {
                flag = true;
                break;
            }
        }
    return flag;
}
void KeyBoard::KeyBoardTask(void *pragma)
{
    while (1)
    {
        if (_KeyBoard->isPressed())
        {
            switch (_KeyBoard->_kbDate.key)
            {
                case 119: //W
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_ANALOG_LEFT_Y]++;
                    break;
                case 115: //S
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_ANALOG_LEFT_Y]--;
                    break;
                case 97: //A
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_ANALOG_RIGHT_X]--;
                    break;
                case 100: //D
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_ANALOG_RIGHT_X]++;
                case 113: //Q
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_ANALOG_LEFT_X]--;
                    break;
                case 101: //E
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_ANALOG_LEFT_X]++;
                    break;
                case 38: //Up Arrow
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_ANALOG_RIGHT_Y]--;
                    break;
                case 40: //Dw Arrow
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_ANALOG_RIGHT_Y]++;
                    break;
                case 114: //R
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_UP - 2] = 1;
                    break;
                case 102: //F
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_DOWN - 2] = 1;
                    break;
                case 116: //T
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_LEFT - 2] = 1;
                    break;
                case 103: //G
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_RIGHT - 2] = 1;
                case 121: //y
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_X - 2] = 1;
                    break;
                case 104: //h
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_B - 2] = 1;
                case 117: //u
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_Y - 2] = 1;
                    break;
                case 106: //j
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_A - 2] = 1;
                    break;
                case 122: //z
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_L1 - 2] = 1;
                    break;
                case 120: //x
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_L2 - 2] = 1;
                case 99: //c
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_R1 - 2] = 1;
                    break;
                case 118: //v
                    _KeyBoard->_btnVal[pros::E_CONTROLLER_DIGITAL_R2 - 2] = 1;
                    break;
            }
        }
        else
            _KeyBoard->_btnVal.fill(0);
        keyboard_read(&_KeyBoard->_real_kb_drv, &_KeyBoard->_kbDate);
        pros::delay(10);
    }
}
} // namespace ncrapi
#else
#endif
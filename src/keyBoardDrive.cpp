#include "./ncrLvglSimKernel.hpp"
#if USE_PROS_LVGL_SIM == 1
#include "keyBoardDrive.hpp"
#include "pros/misc.h"
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
    _lastKbVal = 0;
    _kbDate.key = 50;
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
int KeyBoard::getKeyBoardAnalog(const KEYBOARD_ANALOG ch)
{
    if (isPressed())
    {
        switch (_kbDate.key)
        {
            case 119: //W
                _analogVal[pros::E_CONTROLLER_ANALOG_LEFT_Y]++;
                break;
            case 115: //S
                _analogVal[pros::E_CONTROLLER_ANALOG_LEFT_Y]--;
                break;
            case 97: //A
                _analogVal[pros::E_CONTROLLER_ANALOG_RIGHT_X]--;
                break;
            case 100: //D
                _analogVal[pros::E_CONTROLLER_ANALOG_RIGHT_X]++;
            case 113: //Q
                _analogVal[pros::E_CONTROLLER_ANALOG_LEFT_X]--;
                break;
            case 101: //E
                _analogVal[pros::E_CONTROLLER_ANALOG_LEFT_X]++;
                break;
            case 38: //Up Arrow
                _analogVal[pros::E_CONTROLLER_ANALOG_RIGHT_Y]--;
                break;
            case 40: //Dw Arrow
                _analogVal[pros::E_CONTROLLER_ANALOG_RIGHT_Y]++;
                break;
        }
    }
    else
        _analogVal.fill(0);
    return _analogVal[ch];
}
bool KeyBoard::getKeyBoradDigtal(const KEYBOARD_DIGITAL btn)
{
    bool flag = false;
    if (isPressed())
    {
        if (btn == _kbDate.key)
            flag = true;
    }
    return flag;
}
bool KeyBoard::getNewPrssed(const KEYBOARD_DIGITAL btn)
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

} // namespace ncrapi
#else
#endif
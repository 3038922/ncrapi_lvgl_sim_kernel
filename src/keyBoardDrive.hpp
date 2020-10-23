#pragma once
#include "./ncrLvglSimKernel.hpp"
#if USE_PROS_LVGL_SIM == 1
#include "../lvgl/lvgl.h"

namespace ncrapi {
#define KEYBOARD_NO_PR 0 //没有按钮按下默认255
enum KEYBOARD_ANALOG {
    KEYBOARD_ANALOG_CH1 = 0,
    KEYBOARD_ANALOG_CH2 = 1,
    KEYBOARD_ANALOG_CH3 = 2,
    KEYBOARD_ANALOG_CH4 = 3,
};
enum KEYBOARD_DIGITAL {
    //ANAGLOG
    KEYBOARD_ANALOG_Up = 38,
    KEYBOARD_ANALOG_DW = 40,
    KEYBOARD_ANALOG_A = 97,
    KEYBOARD_ANALOG_D = 100,
    KEYBOARD_ANALOG_E = 101,
    KEYBOARD_ANALOG_Q = 113,
    //DIGITAL
    KEYBOARD_DIGITAL_1 = 49,
    KEYBOARD_DIGITAL_2 = 50,
    KEYBOARD_DIGITAL_3 = 51,
    KEYBOARD_DIGITAL_4 = 52,
    KEYBOARD_ANALOG_S = 115,
    KEYBOARD_ANALOG_W = 119,
    KEYBOARD_DIGITAL_C = 99,
    KEYBOARD_DIGITAL_F = 102,
    KEYBOARD_DIGITAL_G = 103,
    KEYBOARD_DIGITAL_H = 104,
    KEYBOARD_DIGITAL_J = 106,
    KEYBOARD_DIGITAL_R = 114,

    KEYBOARD_DIGITAL_T = 116,
    KEYBOARD_DIGITAL_U = 117,
    KEYBOARD_DIGITAL_V = 118,

    KEYBOARD_DIGITAL_X = 120,

    KEYBOARD_DIGITAL_Y = 121,

    KEYBOARD_DIGITAL_Z = 122,

};
class KeyBoard
{
  public:
    //饿汉模式单例实现.线程安全
    static KeyBoard *initKeyBoard();
    bool isPressed();
    bool getFiledDigital(const KEYBOARD_DIGITAL btn);
    bool getFiledNewPressed(const KEYBOARD_DIGITAL btn);
    int getKeyBoardAnalog(const KEYBOARD_ANALOG ch);

    bool getKeyBoardDigtal(const KEYBOARD_DIGITAL btn);
    bool getNewPressed(const KEYBOARD_DIGITAL btn);
    static void KeyBoardTask(void *pragma);
    static KeyBoard *_KeyBoard; // 单例对象
  private:
    KeyBoard();
    ~KeyBoard() {}

    lv_indev_data_t _kbDate;     //键盘数据
    std::array<int, 16> _btnVal; //0 leftX ,1 leftY ,2  rightX ,3 _rightY;
    lv_indev_drv_t _real_kb_drv;
};

} // namespace ncrapi
#else
#endif
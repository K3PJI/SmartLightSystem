#include <input.h>
#include <Arduino.h>

#define ENC_A 2
#define ENC_B 7
#define ENC_SW A0
#define BTN A1

#define btn_debounse_ms 40
#define btn_hold_ms 700

bool enc_left = false;
bool enc_right = false;
bool enc_click = false;
bool enc_hold = false;
bool btn_click = false;
bool btn_hold = false;

static uint8_t lastA = HIGH;
static uint8_t lastB = HIGH;

static bool encBtnWasPressed = false;
static unsigned long encBtnDownTime = 0;
static unsigned long encBtnLastChangeTime = 0;
static bool encBtnStable = HIGH;

static bool btnWasPressed = false;
static unsigned long btnDownTime = 0;
static unsigned long btnLastChangeTime = 0;
static bool btnStable = HIGH;

void input_init()
{
    pinMode(ENC_A, INPUT_PULLUP);
    pinMode(ENC_B, INPUT_PULLUP);
    pinMode(ENC_SW, INPUT_PULLUP);
    pinMode(BTN, INPUT_PULLUP);

    lastA = digitalRead(ENC_A);
    lastB = digitalRead(ENC_B);
    encBtnStable = digitalRead(ENC_SW);
    btnStable = digitalRead(BTN);
}

void input_update()
{
    unsigned long now = 0;

    uint8_t a = digitalRead(ENC_A);
    uint8_t b = digitalRead(ENC_B);

    if (a != lastA && a == LOW)
    {
        if (b == HIGH)
        {
            enc_right = true;
        }
        else
        {
            enc_left = true;
        }
    }
    lastA = a;

    bool rawEncBtn = digitalRead(ENC_SW);

    if (rawEncBtn != encBtnStable)
    {
        if (now - encBtnLastChangeTime > btn_debounse_ms)
        {
            encBtnStable = rawEncBtn;
            encBtnLastChangeTime = now;

            if (encBtnStable == LOW)
            {
                encBtnWasPressed = true;
                encBtnDownTime = now;
            }
            else
            {
                if (encBtnWasPressed)
                {
                    unsigned long dur = now - encBtnDownTime;
                    if (dur < btn_hold_ms)
                    {
                        enc_click = true;
                    }
                    encBtnWasPressed = false;
                }
            }
        }
    }
    else
    {
        if (encBtnWasPressed && (now - encBtnDownTime > btn_hold_ms))
        {
            enc_hold = true;
            encBtnWasPressed = false;
        }
    }

    bool rawBtn = digitalRead(BTN);

    if (rawBtn != btnStable)
    {
        if (now - btnLastChangeTime > btn_debounse_ms)
        {
            btnStable = rawBtn;
            btnLastChangeTime = now;

            if (btnStable == LOW)
            {
                btnWasPressed = true;
                btnDownTime = now;
            }
            else
            {
                if (btnWasPressed)
                {
                    unsigned long dur = now - btnDownTime;
                    if (dur < btn_hold_ms)
                    {
                        btn_click = true;
                    }
                    btnWasPressed = false;
                }
            }
        }
    }
    else
    {
        if (btnWasPressed && (now - btnDownTime > btn_hold_ms))
        {
            btn_hold = true;
            btnWasPressed = false;
        }
    }
}
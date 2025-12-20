#include "menu_manager.h"
#include "input.h"
#include "sensors.h"
#include "led_strip.h"
#include "storage.h"

extern SystemState State;
extern Settings CurrentSettings;
extern unsigned long lastActivityTime;

const char *MAIN_MENU_ITEMS[] = {"Подсветка п.", "Подсветка м.", "Настройки"};
#define MAIN_MENU_SIZE 3

const char *LIGHT_SETTINGS_ITEMS[] = {"Выбор режима", "Статичный цвет", "Яркость"};
#define LIGHT_SETTINGS_SIZE 3

const char *CEILING_MODES_ITEMS[] = {"Спокойствие", "Радуга", "Импульс"};
#define CEILING_MODES_SIZE CEILING_MODE_COUNT

const char *STATIC_COLOR_ITEMS[] = {"Холодный", "Теплый", "Пользовательский"};
#define STATIC_COLOR_SIZE COLOR_PRESET_COUNT

const char *COLOR_PICKER_ITEMS[] = {"Оттенок", "Насыщенность", "Яркость"};
#define COLOR_PICKER_SIZE 3

const char *SETTINGS_MENU_ITEMS[] = {"Метеостанция", "Контраст экрана", "О продукте"};
#define SETTINGS_MENU_SIZE 3

const char *CALIBRATION_ITEMS[] = {"Температура", "Влажность"};
#define CALIBRATION_SIZE 2

void User_Activity_Detected()
{
    lastActivityTime = millis();
    if (Display_IsSleeping())
    {
        Display_Wake();
    }
    Display_SetNeedsUpdate(true);
}

void Save_And_Apply_Settings()
{
    Led_SetCeilingBrightness(CurrentSettings.ceilingBrightness);
    Led_SetCeilingMode(CurrentSettings.lastCeilingMode);
    Led_SetCeilingHSV(CurrentSettings.hue, CurrentSettings.saturation, CurrentSettings.value);
    Led_SetMonitorBrightness(CurrentSettings.monitorBrightness);
    Led_SetMonitorPower(CurrentSettings.monitorEnabled);

    Storage_SaveSettings(&CurrentSettings);
}

static void Handle_Navigation(uint8_t maxSize)
{
    if (enc_left)
    {
        State.selected_menu_item = (State.selected_menu_item + maxSize - 1) % maxSize;
    }
    if (enc_right)
    {
        State.selected_menu_item = (State.selected_menu_item + 1) % maxSize;
    }
}

void Menu_Manager_Update()
{
    if (!(enc_right || enc_left || enc_click || enc_hold || btn_click || btn_hold))
    {
        return;
    }

    User_Activity_Detected();

    if (btn_hold)
    {
        CurrentSettings.monitorEnabled = !CurrentSettings.monitorEnabled;
        Save_And_Apply_Settings();
    }
    if (btn_click)
    {
        Led_ToggleMonitorColor();
        Save_And_Apply_Settings();
    }

    if (enc_hold)
    {
        switch (State.mode)
        {
        case MODE_MAIN_MENU:
            State.mode = MODE_START_SCREEN;
            break;
        case MODE_LIGHT_SETTINGS:
        case MODE_MONITOR_SETTINGS:
        case MODE_SETTINGS_MENU:
        case MODE_ABOUT:
            State.mode = MODE_MAIN_MENU;
            break;
        case MODE_CEILING_MODE_SELECTION:
        case MODE_STATIC_COLOR_SELECTION:
        case MODE_BRIGHTNESS_ADJUST:
        case MODE_COLOR_PICKER:
            State.mode = MODE_LIGHT_SETTINGS;
            break;
        case MODE_CALIBRATION_MENU:
        case MODE_CONTRAST_ADJUST:
            State.mode = MODE_SETTINGS_MENU;
            break;
        case MODE_COLOR_PICKER_HUE:
        case MODE_COLOR_PICKER_SAT:
        case MODE_COLOR_PICKER_VAL:
            State.mode = MODE_COLOR_PICKER;
            break;
        case MODE_CALIBRATION_TEMP:
        case MODE_CALIBRATION_HUM:
            State.mode = MODE_CALIBRATION_MENU;
            break;
        default:
            break;
        }
        State.selected_menu_item = 0;
        goto cleanup_flags;
    }

    switch (State.mode)
    {
    case MODE_START_SCREEN:
        if (enc_click)
        {
            State.mode = MODE_MAIN_MENU;
        }
        break;

    case MODE_MAIN_MENU:
        Handle_Navigation(MAIN_MENU_SIZE);
        if (enc_click)
        {
            switch (State.selected_menu_item)
            {
            case 0:
                State.mode = MODE_LIGHT_SETTINGS;
                break;
            case 1:
                State.mode = MODE_MONITOR_SETTINGS;
                break;
            case 2:
                State.mode = MODE_SETTINGS_MENU;
                break;
            }
            State.selected_menu_item = 0;
        }
        break;

    case MODE_LIGHT_SETTINGS:
        Handle_Navigation(LIGHT_SETTINGS_SIZE);
        if (enc_click)
        {
            switch (State.selected_menu_item)
            {
            case 0:
                State.mode = MODE_CEILING_MODE_SELECTION;
                State.selected_menu_item = CurrentSettings.lastCeilingMode;
                break;
            case 1:
                State.mode = MODE_STATIC_COLOR_SELECTION;
                State.selected_menu_item = CurrentSettings.staticColorPreset;
                break;
            case 2:
                State.mode = MODE_BRIGHTNESS_ADJUST;
                break;
            }
        }
        break;

    case MODE_STATIC_COLOR_SELECTION:
        Handle_Navigation(STATIC_COLOR_SIZE);
        if (enc_click)
        {
            CurrentSettings.staticColorPreset = (StaticColorPreset)State.selected_menu_item;
            if (CurrentSettings.staticColorPreset == COLOR_PRESET_CUSTOM)
            {
                State.mode = MODE_COLOR_PICKER;
            }
            else
            {
                Led_SetCeilingColorPreset(CurrentSettings.staticColorPreset);
                Save_And_Apply_Settings();
                State.mode = MODE_LIGHT_SETTINGS;
            }
        }
        break;

    case MODE_COLOR_PICKER: // Меню H/S/V
        Handle_Navigation(COLOR_PICKER_SIZE);
        if (enc_click)
        {
            switch (State.selected_menu_item)
            {
            case 0:
                State.mode = MODE_COLOR_PICKER_HUE;
                break;
            case 1:
                State.mode = MODE_COLOR_PICKER_SAT;
                break;
            case 2:
                State.mode = MODE_COLOR_PICKER_VAL;
                break;
            }
        }
        break;

    case MODE_SETTINGS_MENU:
        Handle_Navigation(SETTINGS_MENU_SIZE);
        if (enc_click)
        {
            switch (State.selected_menu_item)
            {
            case 0:
                State.mode = MODE_CALIBRATION_MENU;
                break;
            case 1:
                State.mode = MODE_CONTRAST_ADJUST;
                break;
            case 2:
                State.mode = MODE_ABOUT;
                break;
            }
            State.selected_menu_item = 0;
        }
        break;

    case MODE_CALIBRATION_MENU:
        Handle_Navigation(CALIBRATION_SIZE);
        if (enc_click)
        {
            switch (State.selected_menu_item)
            {
            case 0:
                State.mode = MODE_CALIBRATION_TEMP;
                break;
            case 1:
                State.mode = MODE_CALIBRATION_HUM;
                break;
            }
        }
        break;

    case MODE_CEILING_MODE_SELECTION:
        Handle_Navigation(CEILING_MODES_SIZE);
        if (enc_click)
        {
            CurrentSettings.lastCeilingMode = (CeilingMode)State.selected_menu_item;
            Save_And_Apply_Settings();
            State.mode = MODE_LIGHT_SETTINGS;
        }
        break;

    case MODE_BRIGHTNESS_ADJUST:
        if (enc_left)
            CurrentSettings.ceilingBrightness = constrain(CurrentSettings.ceilingBrightness - 5, 0, 255);
        if (enc_right)
            CurrentSettings.ceilingBrightness = constrain(CurrentSettings.ceilingBrightness + 5, 0, 255);
        Led_SetCeilingBrightness(CurrentSettings.ceilingBrightness);
        if (enc_click)
        {
            Save_And_Apply_Settings();
            State.mode = MODE_LIGHT_SETTINGS;
        }
        break;

    case MODE_MONITOR_SETTINGS: // Яркость мониторной подсветки (0-255, шаг 5)
        if (enc_left)
            CurrentSettings.monitorBrightness = constrain(CurrentSettings.monitorBrightness - 5, 0, 255);
        if (enc_right)
            CurrentSettings.monitorBrightness = constrain(CurrentSettings.monitorBrightness + 5, 0, 255);
        Led_SetMonitorBrightness(CurrentSettings.monitorBrightness);
        if (enc_click)
        {
            Save_And_Apply_Settings();
            State.mode = MODE_MAIN_MENU;
        }
        break;

    case MODE_COLOR_PICKER_HUE:
        if (enc_left)
            CurrentSettings.hue = (CurrentSettings.hue - 512) % 65536;
        if (enc_right)
            CurrentSettings.hue = (CurrentSettings.hue + 512) % 65536;
        Led_SetCeilingHSV(CurrentSettings.hue, CurrentSettings.saturation, CurrentSettings.value);
        if (enc_click)
        {
            Save_And_Apply_Settings();
            State.mode = MODE_COLOR_PICKER;
        }
        break;

    case MODE_COLOR_PICKER_SAT: // Насыщенность (0-255, шаг 5)
        if (enc_left)
            CurrentSettings.saturation = constrain(CurrentSettings.saturation - 5, 0, 255);
        if (enc_right)
            CurrentSettings.saturation = constrain(CurrentSettings.saturation + 5, 0, 255);
        Led_SetCeilingHSV(CurrentSettings.hue, CurrentSettings.saturation, CurrentSettings.value);
        if (enc_click)
        {
            Save_And_Apply_Settings();
            State.mode = MODE_COLOR_PICKER;
        }
        break;

    case MODE_COLOR_PICKER_VAL: // Яркость V (0-255, шаг 5)
        if (enc_left)
            CurrentSettings.value = constrain(CurrentSettings.value - 5, 0, 255);
        if (enc_right)
            CurrentSettings.value = constrain(CurrentSettings.value + 5, 0, 255);
        Led_SetCeilingHSV(CurrentSettings.hue, CurrentSettings.saturation, CurrentSettings.value);
        if (enc_click)
        {
            Save_And_Apply_Settings();
            State.mode = MODE_COLOR_PICKER;
        }
        break;

    case MODE_CALIBRATION_TEMP:
        if (enc_left)
            CurrentSettings.tempOffset -= 0.1;
        if (enc_right)
            CurrentSettings.tempOffset += 0.1;
        Sensors_Update();
        if (enc_click)
        {
            Save_And_Apply_Settings();
            State.mode = MODE_CALIBRATION_MENU;
        }
        break;

    case MODE_CALIBRATION_HUM:
        if (enc_left)
            CurrentSettings.humOffset -= 1.0;
        if (enc_right)
            CurrentSettings.humOffset += 1.0;
        Sensors_Update();
        if (enc_click)
        {
            Save_And_Apply_Settings();
            State.mode = MODE_CALIBRATION_MENU;
        }
        break;

    case MODE_CONTRAST_ADJUST:
        if (enc_left)
            CurrentSettings.displayContrast = constrain(CurrentSettings.displayContrast - 1, 0, 100);
        if (enc_right)
            CurrentSettings.displayContrast = constrain(CurrentSettings.displayContrast + 1, 0, 100);
        if (enc_click)
        {
            Save_And_Apply_Settings();
            State.mode = MODE_SETTINGS_MENU;
        }
        break;

    case MODE_ABOUT:
        if (enc_click)
        {
            State.mode = MODE_SETTINGS_MENU;
        }
        break;

    default:
        break;
    }

cleanup_flags:
    enc_right = enc_left = enc_click = enc_hold = false;
    btn_click = btn_hold = false;
}
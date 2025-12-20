#include "display.h"
#include "config.h"
#include "storage.h"
#include "sensors.h"
#include "menu_manager.h"
#include <U8g2lib.h>

U8G2_GMG12864_06D_1_4W_SPI u8g2(U8G2_R0, PIN_LCD_SCL, PIN_LCD_SI, PIN_LCD_CS, PIN_LCD_RS, U8X8_PIN_NONE);

extern SystemState State;
extern Settings CurrentSettings;

static bool isDisplaySleeping = false;
static bool needsUpdate = true;

#define LINE_HEIGHT 12
#define CURSOR_W 4
#define CURSOR_H (LINE_HEIGHT - 2)
#define CURSOR_X_OFFSET 2
#define MENU_START_Y 15

void Display_SetContrast(uint8_t contrastPercent)
{
    uint8_t u8g2_contrast = map(contrastPercent, 0, 100, 0, 255);
    u8g2.setContrast(u8g2_contrast);
}

void Display_Sleep()
{
    if (!isDisplaySleeping)
    {
        u8g2.setPowerSave(1);
        isDisplaySleeping = true;
    }
}

void Display_Wake()
{
    if (isDisplaySleeping)
    {
        u8g2.setPowerSave(0);
        isDisplaySleeping = false;
        Display_SetNeedsUpdate(true);
    }
}

bool Display_IsSleeping()
{
    return isDisplaySleeping;
}

void Display_SetNeedsUpdate(bool update)
{
    needsUpdate = update;
}

static void DrawStartScreen()
{
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.setFontMode(1);

    float temp = Sensors_GetTemperature();
    float hum = Sensors_GetHumidity();
    SensorStatus status = Sensors_GetStatus();

    if (status == SENSOR_OK)
    {
        u8g2.drawStr(0, LINE_HEIGHT * 1, "T:");
        u8g2.setCursor(15, LINE_HEIGHT * 1);
        u8g2.print(temp, 1);
        u8g2.drawStr(45, LINE_HEIGHT * 1, "C");

        u8g2.drawStr(65, LINE_HEIGHT * 1, "H:");
        u8g2.setCursor(80, LINE_HEIGHT * 1);
        u8g2.print((int)hum);
        u8g2.drawStr(105, LINE_HEIGHT * 1, "%");
    }
    else
    {
        u8g2.drawStr(0, LINE_HEIGHT * 1, "Датчик: ошибка");
    }

    u8g2.drawStr(0, LINE_HEIGHT * 2, "Режим:");
    const char *modeName = CEILING_MODES_ITEMS[CurrentSettings.lastCeilingMode];
    u8g2.drawStr(50, LINE_HEIGHT * 2, modeName);

    uint8_t brightness_pct = map(CurrentSettings.ceilingBrightness, 0, 255, 0, 100);
    u8g2.drawStr(0, LINE_HEIGHT * 3, "Ярк. п.:");
    u8g2.setCursor(60, LINE_HEIGHT * 3);
    u8g2.print(brightness_pct);
    u8g2.drawStr(90, LINE_HEIGHT * 3, "%");

    u8g2.drawStr(0, LINE_HEIGHT * 4, "Монитор:");
    u8g2.drawStr(60, LINE_HEIGHT * 4, CurrentSettings.monitorEnabled ? "ВКЛ" : "ВЫКЛ");

    u8g2.setFont(u8g2_font_4x6_tf);
    u8g2.drawStr(0, LINE_HEIGHT * 5, "Нажмите для меню");
}

static void DrawGenericMenu(const char *title, const char **items, uint8_t count, uint8_t selected_index)
{
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.setFontMode(1);

    u8g2.drawStr(0, LINE_HEIGHT, title);
    u8g2.drawHLine(0, LINE_HEIGHT + 1, u8g2.getDisplayWidth());

    for (int i = 0; i < 4; i++)
    {
        int itemIndex = selected_index - 1 + i;
        if (item_index >= 0 && itemIndex < count)
        {
            int y = MENU_START_Y + LINE_HEIGHT * i;

            if (item_index == selected_index)
            {
                u8g2.drawBox(0, y - CURSOR_H / 2, u8g2.getDisplayWidth(), CURSOR_H + 2);
                u8g2.setDrawColor(0);
                u8g2.drawStr(CURSOR_X_OFFSET, y, items[itemIndex]);
                u8g2.setDrawColor(1);
            }
            else
            {
                u8g2.drawStr(CURSOR_X_OFFSET, y, items[itemIndex]);
            }
        }
    }
}

static void DrawValueAdjust(const char *title, float value, float max_value, const char *value_unit)
{
    u8g2.setFont(u8g2_font_7x13B_tf);
    u8g2.setFontMode(1);

    u8g2.drawStr(0, LINE_HEIGHT, title);

    u8g2.setFont(u8g2_font_fub20_tf);
    u8g2.setCursor(0, 4 * LINE_HEIGHT);

    if (value_unit[0] == 'C' || value_unit[0] == 'H')
    {
        u8g2.print(value, 1);
    }
    else
    {
        u8g2.print((int)value);
    }
    u8g2.drawStr(u8g2.getCursorX(), 4 * LINE_HEIGHT, value_unit);

    int slider_width = u8g2.getDisplayWidth() - 4;
    int slider_x = 2;
    int slider_y = u8g2.getDisplayHeight() - 10;

    u8g2.drawFrame(slider_x, slider_y, slider_width, 8);

    float ratio = value / max_value;
    if (max_value == 0)
    {
        ratio = 0;
    }
    int fill_width = (int)(slider_width * ratio);

    u8g2.drawBox(slider_x, slider_y, fill_width, 8);
}

static void DrawAboutScreen()
{
    u8g2.setFont(u8g2_font_6x12_tf);
    u8g2.drawStr(0, LINE_HEIGHT * 1, "SmartLight System");
    u8g2.drawStr(0, LINE_HEIGHT * 2, "Версия: 1.0");
    u8g2.drawStr(0, LINE_HEIGHT * 3, "Процесстор: ATmega328P");
    u8g2.drawStr(0, LINE_HEIGHT * 4, "T/H Сенсор: DHT22");
    u8g2.drawStr(0, LINE_HEIGHT * 6, "Нажмите для выхода");
}

void Display_UpdateScreen()
{
    if (isDisplaySleeping)
    {
        needsUpdate = false;
        return;
    }

    if (!needsUpdate)
    {
        return;
    }

    u8g2.firstPage();
    do
    {
        u8g2.setDrawColor(1);

        switch (State.mode)
        {
        case MODE_START_SCREEN:
            DrawStartScreen();
            break;

        case MODE_MAIN_MENU:
            DrawGenericMenu("ГЛАВНОЕ МЕНЮ", MAIN_MENU_ITEMS, MAIN_MENU_SIZE, State.selected_menu_item);
            break;

        case MODE_LIGHT_SETTINGS:
            DrawGenericMenu("ПОДСВЕТКА ПОТОЛКА", LIGHT_SETTINGS_ITEMS, LIGHT_SETTINGS_SIZE, State.selected_menu_item);

        case MODE_CILING_MODE_SELECTION:
            DrawGenericMenu("ВЫБОР РЕЖИМА", CEILING_MODES_ITEMS, CEILING_MODES_SIZE, State.selected_menu_item);
            break;

        case MODE_STATIC_COLOR_SELECTION:
            DrawGenericMenu("СТАТИЧНЫЙ ЦВЕТ", STATIC_COLOR_ITEMS, STATIC_COLOR_SIZE, State.selected_menu_item);
            break;

        case MODE_COLOR_PICKER:
            DrawGenericMenu("ПОЛЬЗ. ЦВЕТ (H/S/V)", COLOR_PICKER_ITEMS, COLOR_PICKER_SIZE, State.selected_menu_item);
            break;

        case MODE_SETTINGS_MENU:
            DrawGenericMenu("НАСТРОЙКИ СИСТЕМЫ", SETTINGS_MENU_ITEMS, SETTINGS_MENU_SIZE, State.selected_menu);
            break;

        case MODE_CALIBRATION_MENU:
            DrawGenericMenu("КАЛИБРОВКА (T/H)", CALIBRATION_ITEMS, CALIBRATION_SIZE, State.selected_menu_item);
            break;

        case MODE_BRIGHTNESS_ADJUST:
            DrawValueAdjust("Яркость потолка", map(CurrentSettings.ceilingBrightness, 0, 255, 0, 100), 100, "%");
            break;

        case MODE_MONITOR_SETTINGS:
            DrawValueAdjust("Яркость монитора", map(CurrentSettings.monitorBrightness, 0, 255, 0, 100), 100, "%");
            break;

        case MODE_COLOR_PICKER_HUE:
            DrawValueAdjust("Оттенок (H)", map(CurrentSettings.hue, 0, 65535, 0, 360), 360, "deg");
            break;

        case MODE_COLOR_PICKER_SAT:
            DrawValueAdjust("Насыщенность (S)", map(CurrentSettings.saturation, 0, 255, 0, 100), 100, "%");
            break;

        case MODE_COLOR_PICKER_VAL:
            DrawValueAdjust("Яркость (V)", map(CurrentSettings.value, 0, 255, 0, 100), 100, "%");
            break;

        case MODE_CALIBRATION_HUM:
            DrawValueAdjust("Смещение Hum.", CurrentSettings.humOffset, 10.0, "%");
            break;

        case MODE_ABOUT:
            DrawAboutScreen();
            break;

        default:
            u8g2.drawStr(0, 10, "UNKNOWN STATE");
            break;
        }
    } while (u8g2.nextPage())

        needsUpdate = false;
}

void Display_Init()
{
    u8g2.begin();
    u8g2.setFlipMode(0);
    u8g2.setFontMode(1);

    Display_SetContrast(CurrentSettings.displayContrast);

    Display_SetNeedsUpdate(true);
}
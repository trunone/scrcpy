#ifndef SC_SCREEN_OTG_WIN32_H
#define SC_SCREEN_OTG_WIN32_H

#include "common.h"

#include <stdbool.h>
#include <windows.h>

#include "usb/gamepad_aoa.h"
#include "usb/keyboard_aoa.h"
#include "usb/mouse_aoa.h"

struct sc_screen_otg_win32 {
    struct sc_keyboard_aoa *keyboard;
    struct sc_mouse_aoa *mouse;
    struct sc_gamepad_aoa *gamepad;

    HWND hwnd;

    // Mouse capture state
    bool mouse_captured;
    uint8_t shortcut_mods; // OR of enum sc_shortcut_mod values
};

struct sc_screen_otg_win32_params {
    struct sc_keyboard_aoa *keyboard;
    struct sc_mouse_aoa *mouse;
    struct sc_gamepad_aoa *gamepad;

    const char *window_title;
    bool always_on_top;
    int16_t window_x; // accepts SC_WINDOW_POSITION_UNDEFINED
    int16_t window_y; // accepts SC_WINDOW_POSITION_UNDEFINED
    uint16_t window_width;
    uint16_t window_height;
    bool window_borderless;
    uint8_t shortcut_mods; // OR of enum sc_shortcut_mod values
};

bool
sc_screen_otg_win32_init(struct sc_screen_otg_win32 *screen,
                         const struct sc_screen_otg_win32_params *params);

void
sc_screen_otg_win32_destroy(struct sc_screen_otg_win32 *screen);

#endif

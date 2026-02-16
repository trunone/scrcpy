#include "screen_otg_win32.h"

#include <assert.h>
#include <stdio.h>
#include <windows.h>
#include <windowsx.h>

#include "input_events.h"
#include "util/log.h"

#define IDI_ICON 101

static enum sc_scancode
sc_scancode_from_lparam(LPARAM lparam) {
    // Extract scan code from lParam (bits 16-23)
    int scancode = (lparam >> 16) & 0xFF;
    int extended = (lparam >> 24) & 1;

    // Mapping from Windows Scan Code to SDL Scan Code (USB HID Usage)
    // This is a partial mapping covering common keys.
    // Ideally we should use a full table.

    if (extended) {
        switch (scancode) {
            case 0x1C: return SC_SCANCODE_KP_ENTER;
            case 0x1D: return SC_SCANCODE_RCTRL;
            case 0x35: return SC_SCANCODE_KP_DIVIDE;
            case 0x37: return SC_SCANCODE_PRINTSCREEN;
            case 0x38: return SC_SCANCODE_RALT;
            case 0x47: return SC_SCANCODE_HOME;
            case 0x48: return SC_SCANCODE_UP;
            case 0x49: return SC_SCANCODE_PAGEUP;
            case 0x4B: return SC_SCANCODE_LEFT;
            case 0x4D: return SC_SCANCODE_RIGHT;
            case 0x4F: return SC_SCANCODE_END;
            case 0x50: return SC_SCANCODE_DOWN;
            case 0x51: return SC_SCANCODE_PAGEDOWN;
            case 0x52: return SC_SCANCODE_INSERT;
            case 0x53: return SC_SCANCODE_DELETE;
            case 0x5B: return SC_SCANCODE_LGUI;
            case 0x5C: return SC_SCANCODE_RGUI;
            case 0x5D: return SC_SCANCODE_APPLICATION;
        }
    } else {
        switch (scancode) {
            case 0x01: return SC_SCANCODE_ESCAPE;
            case 0x02: return SC_SCANCODE_1;
            case 0x03: return SC_SCANCODE_2;
            case 0x04: return SC_SCANCODE_3;
            case 0x05: return SC_SCANCODE_4;
            case 0x06: return SC_SCANCODE_5;
            case 0x07: return SC_SCANCODE_6;
            case 0x08: return SC_SCANCODE_7;
            case 0x09: return SC_SCANCODE_8;
            case 0x0A: return SC_SCANCODE_9;
            case 0x0B: return SC_SCANCODE_0;
            case 0x0C: return SC_SCANCODE_MINUS;
            case 0x0D: return SC_SCANCODE_EQUALS;
            case 0x0E: return SC_SCANCODE_BACKSPACE;
            case 0x0F: return SC_SCANCODE_TAB;
            case 0x10: return SC_SCANCODE_Q;
            case 0x11: return SC_SCANCODE_W;
            case 0x12: return SC_SCANCODE_E;
            case 0x13: return SC_SCANCODE_R;
            case 0x14: return SC_SCANCODE_T;
            case 0x15: return SC_SCANCODE_Y;
            case 0x16: return SC_SCANCODE_U;
            case 0x17: return SC_SCANCODE_I;
            case 0x18: return SC_SCANCODE_O;
            case 0x19: return SC_SCANCODE_P;
            case 0x1A: return SC_SCANCODE_LEFTBRACKET;
            case 0x1B: return SC_SCANCODE_RIGHTBRACKET;
            case 0x1C: return SC_SCANCODE_RETURN;
            case 0x1D: return SC_SCANCODE_LCTRL;
            case 0x1E: return SC_SCANCODE_A;
            case 0x1F: return SC_SCANCODE_S;
            case 0x20: return SC_SCANCODE_D;
            case 0x21: return SC_SCANCODE_F;
            case 0x22: return SC_SCANCODE_G;
            case 0x23: return SC_SCANCODE_H;
            case 0x24: return SC_SCANCODE_J;
            case 0x25: return SC_SCANCODE_K;
            case 0x26: return SC_SCANCODE_L;
            case 0x27: return SC_SCANCODE_SEMICOLON;
            case 0x28: return SC_SCANCODE_APOSTROPHE;
            case 0x29: return SC_SCANCODE_GRAVE;
            case 0x2A: return SC_SCANCODE_LSHIFT;
            case 0x2B: return SC_SCANCODE_BACKSLASH;
            case 0x2C: return SC_SCANCODE_Z;
            case 0x2D: return SC_SCANCODE_X;
            case 0x2E: return SC_SCANCODE_C;
            case 0x2F: return SC_SCANCODE_V;
            case 0x30: return SC_SCANCODE_B;
            case 0x31: return SC_SCANCODE_N;
            case 0x32: return SC_SCANCODE_M;
            case 0x33: return SC_SCANCODE_COMMA;
            case 0x34: return SC_SCANCODE_PERIOD;
            case 0x35: return SC_SCANCODE_SLASH;
            case 0x36: return SC_SCANCODE_RSHIFT;
            case 0x37: return SC_SCANCODE_KP_MULTIPLY;
            case 0x38: return SC_SCANCODE_LALT;
            case 0x39: return SC_SCANCODE_SPACE;
            case 0x3A: return SC_SCANCODE_CAPSLOCK;
            case 0x3B: return SC_SCANCODE_F1;
            case 0x3C: return SC_SCANCODE_F2;
            case 0x3D: return SC_SCANCODE_F3;
            case 0x3E: return SC_SCANCODE_F4;
            case 0x3F: return SC_SCANCODE_F5;
            case 0x40: return SC_SCANCODE_F6;
            case 0x41: return SC_SCANCODE_F7;
            case 0x42: return SC_SCANCODE_F8;
            case 0x43: return SC_SCANCODE_F9;
            case 0x44: return SC_SCANCODE_F10;
            case 0x45: return SC_SCANCODE_NUMLOCK;
            case 0x46: return SC_SCANCODE_SCROLLLOCK;
            case 0x47: return SC_SCANCODE_KP_7;
            case 0x48: return SC_SCANCODE_KP_8;
            case 0x49: return SC_SCANCODE_KP_9;
            case 0x4A: return SC_SCANCODE_KP_MINUS;
            case 0x4B: return SC_SCANCODE_KP_4;
            case 0x4C: return SC_SCANCODE_KP_5;
            case 0x4D: return SC_SCANCODE_KP_6;
            case 0x4E: return SC_SCANCODE_KP_PLUS;
            case 0x4F: return SC_SCANCODE_KP_1;
            case 0x50: return SC_SCANCODE_KP_2;
            case 0x51: return SC_SCANCODE_KP_3;
            case 0x52: return SC_SCANCODE_KP_0;
            case 0x53: return SC_SCANCODE_KP_PERIOD;
            case 0x57: return SC_SCANCODE_F11;
            case 0x58: return SC_SCANCODE_F12;
        }
    }

    return SC_SCANCODE_UNKNOWN;
}

static uint16_t
get_mods_state(void) {
    uint16_t mods = 0;
    if (GetKeyState(VK_LSHIFT) & 0x8000) mods |= SC_MOD_LSHIFT;
    if (GetKeyState(VK_RSHIFT) & 0x8000) mods |= SC_MOD_RSHIFT;
    if (GetKeyState(VK_LCONTROL) & 0x8000) mods |= SC_MOD_LCTRL;
    if (GetKeyState(VK_RCONTROL) & 0x8000) mods |= SC_MOD_RCTRL;
    if (GetKeyState(VK_LMENU) & 0x8000) mods |= SC_MOD_LALT;
    if (GetKeyState(VK_RMENU) & 0x8000) mods |= SC_MOD_RALT;
    if (GetKeyState(VK_LWIN) & 0x8000) mods |= SC_MOD_LGUI;
    if (GetKeyState(VK_RWIN) & 0x8000) mods |= SC_MOD_RGUI;
    if (GetKeyState(VK_CAPITAL) & 0x0001) mods |= SC_MOD_CAPS;
    if (GetKeyState(VK_NUMLOCK) & 0x0001) mods |= SC_MOD_NUM;
    return mods;
}

static void
handle_key_event(struct sc_screen_otg_win32 *screen, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (!screen->keyboard) {
        return;
    }

    // wparam is VK code
    // lparam contains scan code, repeat count, extended key flag, etc.

    enum sc_action action = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
                          ? SC_ACTION_DOWN : SC_ACTION_UP;
    bool repeat = (lparam & 0x40000000) != 0 && action == SC_ACTION_DOWN;

    enum sc_scancode scancode = sc_scancode_from_lparam(lparam);

    // Fallback for some keys if mapping failed or to handle VK better?
    // But scancode mapping should be robust enough for standard keys.

    struct sc_key_event evt = {
        .action = action,
        .keycode = SC_KEYCODE_UNKNOWN, // AOA uses scancode
        .scancode = scancode,
        .repeat = repeat,
        .mods_state = get_mods_state(),
    };

    struct sc_key_processor *kp = &screen->keyboard->key_processor;
    assert(kp->ops->process_key);
    kp->ops->process_key(kp, &evt, SC_SEQUENCE_INVALID);
}

static void
update_mouse_capture(struct sc_screen_otg_win32 *screen) {
    if (screen->mouse_captured) {
        // Center the cursor
        RECT rect;
        GetClientRect(screen->hwnd, &rect);
        POINT center = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
        ClientToScreen(screen->hwnd, &center);
        SetCursorPos(center.x, center.y);
        screen->mouse_capture_pos = center;

        // Clip cursor
        RECT screenRect;
        GetWindowRect(screen->hwnd, &screenRect);
        ClipCursor(&screenRect);
        ShowCursor(FALSE);
    } else {
        ClipCursor(NULL);
        ShowCursor(TRUE);
    }
}

static void
toggle_mouse_capture(struct sc_screen_otg_win32 *screen) {
    screen->mouse_captured = !screen->mouse_captured;
    update_mouse_capture(screen);
    LOGI("Mouse capture: %s", screen->mouse_captured ? "enabled" : "disabled");
}

static void
handle_mouse_button(struct sc_screen_otg_win32 *screen, enum sc_mouse_button button, bool down) {
    if (!screen->mouse) {
        return;
    }

    if (!screen->mouse_captured && down) {
        // Click to capture
        toggle_mouse_capture(screen);
        return;
    }

    if (!screen->mouse_captured) {
        return;
    }

    struct sc_mouse_processor *mp = &screen->mouse->mouse_processor;

    uint32_t sdl_buttons_state = 0; // TODO: Track button state?
    // sc_mouse_aoa doesn't seem to use buttons_state strongly or we can mock it.
    // Actually sc_hid_mouse_generate_input_from_click uses it.

    // We should track button state in screen struct if needed.
    // For now passing just the current button in state might be enough if AOA handles it?
    // No, AOA sends the full byte 0 with all buttons.

    // Let's implement button state tracking
    static uint8_t buttons_state = 0;
    if (down) {
        buttons_state |= button;
    } else {
        buttons_state &= ~button;
    }

    struct sc_mouse_click_event evt = {
        .action = down ? SC_ACTION_DOWN : SC_ACTION_UP,
        .button = button,
        .buttons_state = buttons_state,
    };

    assert(mp->ops->process_mouse_click);
    mp->ops->process_mouse_click(mp, &evt);
}

static void
handle_raw_input(struct sc_screen_otg_win32 *screen, WPARAM wparam, LPARAM lparam) {
    if (!screen->mouse || !screen->mouse_captured) {
        return;
    }

    UINT size = 0;
    GetRawInputData((HRAWINPUT)lparam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
    if (size == 0) return;

    BYTE *buffer = malloc(size);
    if (!buffer) return;

    if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER)) != size) {
        free(buffer);
        return;
    }

    RAWINPUT *raw = (RAWINPUT*)buffer;
    if (raw->header.dwType == RIM_TYPEMOUSE) {
        int xrel = raw->data.mouse.lLastX;
        int yrel = raw->data.mouse.lLastY;

        if (xrel || yrel) {
            struct sc_mouse_processor *mp = &screen->mouse->mouse_processor;
            struct sc_mouse_motion_event evt = {
                .xrel = xrel,
                .yrel = yrel,
                .buttons_state = 0, // TODO: sync with buttons state
            };
            assert(mp->ops->process_mouse_motion);
            mp->ops->process_mouse_motion(mp, &evt);
        }

        // Handle wheel
        if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
            float amount = (short)raw->data.mouse.usButtonData / (float)WHEEL_DELTA;
            struct sc_mouse_processor *mp = &screen->mouse->mouse_processor;
            struct sc_mouse_scroll_event evt = {
                .vscroll = amount,
                .hscroll = 0,
                .vscroll_int = (int)amount, // Might need accumulation
                .hscroll_int = 0,
                .buttons_state = 0,
            };
             assert(mp->ops->process_mouse_scroll);
             mp->ops->process_mouse_scroll(mp, &evt);
        }
    }

    free(buffer);

    // Re-center cursor if captured
    if (screen->mouse_captured) {
        RECT rect;
        GetClientRect(screen->hwnd, &rect);
        POINT center = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
        ClientToScreen(screen->hwnd, &center);
        SetCursorPos(center.x, center.y);
    }
}

static LRESULT CALLBACK
WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    struct sc_screen_otg_win32 *screen = (struct sc_screen_otg_win32 *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (screen) handle_key_event(screen, msg, wParam, lParam);
            // Handle shortcuts
            if (wParam == VK_MENU && !screen->mouse_captured) {
                 // Check for shortcut mods?
            }
            // Toggle capture shortcut (LAlt, LSuper)
            // Implementation detail: check if shortcut mod is pressed.
            if (screen && (wParam == VK_MENU || wParam == VK_LWIN)) { // Alt or Super
                 if (screen->mouse_captured) {
                     toggle_mouse_capture(screen);
                 }
            }
            return 0;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (screen) handle_key_event(screen, msg, wParam, lParam);
            return 0;
        case WM_INPUT:
            if (screen) handle_raw_input(screen, wParam, lParam);
            return 0;
        case WM_LBUTTONDOWN: if (screen) handle_mouse_button(screen, SC_MOUSE_BUTTON_LEFT, true); return 0;
        case WM_LBUTTONUP: if (screen) handle_mouse_button(screen, SC_MOUSE_BUTTON_LEFT, false); return 0;
        case WM_RBUTTONDOWN: if (screen) handle_mouse_button(screen, SC_MOUSE_BUTTON_RIGHT, true); return 0;
        case WM_RBUTTONUP: if (screen) handle_mouse_button(screen, SC_MOUSE_BUTTON_RIGHT, false); return 0;
        case WM_MBUTTONDOWN: if (screen) handle_mouse_button(screen, SC_MOUSE_BUTTON_MIDDLE, true); return 0;
        case WM_MBUTTONUP: if (screen) handle_mouse_button(screen, SC_MOUSE_BUTTON_MIDDLE, false); return 0;
        case WM_XBUTTONDOWN:
             if (screen) {
                 int button = GET_XBUTTON_WPARAM(wParam);
                 handle_mouse_button(screen, button == XBUTTON1 ? SC_MOUSE_BUTTON_X1 : SC_MOUSE_BUTTON_X2, true);
             }
             return 0;
        case WM_XBUTTONUP:
             if (screen) {
                 int button = GET_XBUTTON_WPARAM(wParam);
                 handle_mouse_button(screen, button == XBUTTON1 ? SC_MOUSE_BUTTON_X1 : SC_MOUSE_BUTTON_X2, false);
             }
             return 0;
        case WM_ACTIVATE:
            if (screen && screen->mouse_captured && LOWORD(wParam) == WA_INACTIVE) {
                toggle_mouse_capture(screen); // Release capture on focus loss
            }
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

bool
sc_screen_otg_win32_init(struct sc_screen_otg_win32 *screen,
                         const struct sc_screen_otg_win32_params *params) {
    screen->keyboard = params->keyboard;
    screen->mouse = params->mouse;
    screen->gamepad = params->gamepad;
    screen->mouse_captured = false;
    screen->shortcut_mods = params->shortcut_mods;

    const char *class_name = "ScrcpyOTG";
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = class_name;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // White background
    // wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON)); // Default icon

    RegisterClass(&wc);

    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    if (params->window_borderless) {
        style = WS_POPUP | WS_VISIBLE;
    }
    if (params->always_on_top) {
        // handled in CreateWindowEx
    }

    int x = params->window_x != SC_WINDOW_POSITION_UNDEFINED ? params->window_x : CW_USEDEFAULT;
    int y = params->window_y != SC_WINDOW_POSITION_UNDEFINED ? params->window_y : CW_USEDEFAULT;
    int w = params->window_width ? params->window_width : 256;
    int h = params->window_height ? params->window_height : 256;

    DWORD exStyle = 0;
    if (params->always_on_top) {
        exStyle |= WS_EX_TOPMOST;
    }

    screen->hwnd = CreateWindowEx(exStyle, class_name, params->window_title, style,
                                  x, y, w, h, NULL, NULL, hInstance, NULL);

    if (!screen->hwnd) {
        LOGE("CreateWindowEx failed: %lu", GetLastError());
        return false;
    }

    SetWindowLongPtr(screen->hwnd, GWLP_USERDATA, (LONG_PTR)screen);

    // Register Raw Input for Mouse
    if (screen->mouse) {
        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01; // Generic Desktop Controls
        rid.usUsage = 0x02;     // Mouse
        rid.dwFlags = 0;
        rid.hwndTarget = screen->hwnd;
        if (!RegisterRawInputDevices(&rid, 1, sizeof(rid))) {
            LOGW("RegisterRawInputDevices failed");
        }
    }

    return true;
}

void
sc_screen_otg_win32_destroy(struct sc_screen_otg_win32 *screen) {
    if (screen->hwnd) {
        DestroyWindow(screen->hwnd);
        screen->hwnd = NULL;
    }
}

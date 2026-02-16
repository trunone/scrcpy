#include "screen_otg.h"

#include <assert.h>
#include <stddef.h>

#include "icon.h"
#include "options.h"
#include "util/acksync.h"
#include "util/log.h"

#ifdef _WIN32
# include <windowsx.h>

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

#define SC_WM_DEVICE_DISCONNECTED (WM_APP + 1)

static void
sc_screen_otg_handle_key_event(struct sc_screen_otg *screen, UINT msg, WPARAM wparam, LPARAM lparam) {
    if (!screen->keyboard) {
        return;
    }

    enum sc_action action = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN)
                          ? SC_ACTION_DOWN : SC_ACTION_UP;
    bool repeat = (lparam & 0x40000000) != 0 && action == SC_ACTION_DOWN;

    enum sc_scancode scancode = sc_scancode_from_lparam(lparam);

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
sc_screen_otg_update_mouse_capture(struct sc_screen_otg *screen) {
    if (screen->mouse_captured) {
        // Center the cursor
        RECT rect;
        GetClientRect(screen->hwnd, &rect);
        POINT center = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
        ClientToScreen(screen->hwnd, &center);
        SetCursorPos(center.x, center.y);

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
sc_screen_otg_toggle_mouse_capture(struct sc_screen_otg *screen) {
    screen->mouse_captured = !screen->mouse_captured;
    sc_screen_otg_update_mouse_capture(screen);
    LOGI("Mouse capture: %s", screen->mouse_captured ? "enabled" : "disabled");
}

static void
sc_screen_otg_handle_mouse_button(struct sc_screen_otg *screen, enum sc_mouse_button button, bool down) {
    if (!screen->mouse) {
        return;
    }

    if (!screen->mouse_captured && down) {
        // Click to capture
        sc_screen_otg_toggle_mouse_capture(screen);
        return;
    }

    if (!screen->mouse_captured) {
        return;
    }

    struct sc_mouse_processor *mp = &screen->mouse->mouse_processor;

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
sc_screen_otg_handle_raw_input(struct sc_screen_otg *screen, WPARAM wparam, LPARAM lparam) {
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
    struct sc_screen_otg *screen = (struct sc_screen_otg *)GetWindowLongPtr(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (screen) sc_screen_otg_handle_key_event(screen, msg, wParam, lParam);
            // Toggle capture shortcut (LAlt, LSuper)
            if (screen && (wParam == VK_MENU || wParam == VK_LWIN)) { // Alt or Super
                 if (screen->mouse_captured) {
                     sc_screen_otg_toggle_mouse_capture(screen);
                 }
            }
            return 0;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if (screen) sc_screen_otg_handle_key_event(screen, msg, wParam, lParam);
            return 0;
        case WM_INPUT:
            if (screen) sc_screen_otg_handle_raw_input(screen, wParam, lParam);
            return 0;
        case WM_LBUTTONDOWN: if (screen) sc_screen_otg_handle_mouse_button(screen, SC_MOUSE_BUTTON_LEFT, true); return 0;
        case WM_LBUTTONUP: if (screen) sc_screen_otg_handle_mouse_button(screen, SC_MOUSE_BUTTON_LEFT, false); return 0;
        case WM_RBUTTONDOWN: if (screen) sc_screen_otg_handle_mouse_button(screen, SC_MOUSE_BUTTON_RIGHT, true); return 0;
        case WM_RBUTTONUP: if (screen) sc_screen_otg_handle_mouse_button(screen, SC_MOUSE_BUTTON_RIGHT, false); return 0;
        case WM_MBUTTONDOWN: if (screen) sc_screen_otg_handle_mouse_button(screen, SC_MOUSE_BUTTON_MIDDLE, true); return 0;
        case WM_MBUTTONUP: if (screen) sc_screen_otg_handle_mouse_button(screen, SC_MOUSE_BUTTON_MIDDLE, false); return 0;
        case WM_XBUTTONDOWN:
             if (screen) {
                 int button = GET_XBUTTON_WPARAM(wParam);
                 sc_screen_otg_handle_mouse_button(screen, button == XBUTTON1 ? SC_MOUSE_BUTTON_X1 : SC_MOUSE_BUTTON_X2, true);
             }
             return 0;
        case WM_XBUTTONUP:
             if (screen) {
                 int button = GET_XBUTTON_WPARAM(wParam);
                 sc_screen_otg_handle_mouse_button(screen, button == XBUTTON1 ? SC_MOUSE_BUTTON_X1 : SC_MOUSE_BUTTON_X2, false);
             }
             return 0;
        case WM_ACTIVATE:
            if (screen && screen->mouse_captured && LOWORD(wParam) == WA_INACTIVE) {
                sc_screen_otg_toggle_mouse_capture(screen); // Release capture on focus loss
            }
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static bool
sc_screen_otg_init_win32(struct sc_screen_otg *screen,
                         const struct sc_screen_otg_params *params) {
    screen->mouse_captured = false;

    const char *class_name = "ScrcpyOTG";
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = class_name;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // White background

    RegisterClass(&wc);

    DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    if (params->window_borderless) {
        style = WS_POPUP | WS_VISIBLE;
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
#endif // _WIN32

static void
sc_screen_otg_render(struct sc_screen_otg *screen) {
    SDL_RenderClear(screen->renderer);
    if (screen->texture) {
        SDL_RenderCopy(screen->renderer, screen->texture, NULL, NULL);
    }
    SDL_RenderPresent(screen->renderer);
}

bool
sc_screen_otg_init(struct sc_screen_otg *screen,
                   const struct sc_screen_otg_params *params) {
    screen->keyboard = params->keyboard;
    screen->mouse = params->mouse;
    screen->gamepad = params->gamepad;

#ifdef _WIN32
    screen->native = params->native;
    if (screen->native) {
        return sc_screen_otg_init_win32(screen, params);
    }
#endif

    const char *title = params->window_title;
    assert(title);

    int x = params->window_x != SC_WINDOW_POSITION_UNDEFINED
          ? params->window_x : (int) SDL_WINDOWPOS_UNDEFINED;
    int y = params->window_y != SC_WINDOW_POSITION_UNDEFINED
          ? params->window_y : (int) SDL_WINDOWPOS_UNDEFINED;
    int width = params->window_width ? params->window_width : 256;
    int height = params->window_height ? params->window_height : 256;

    uint32_t window_flags = SDL_WINDOW_ALLOW_HIGHDPI;
    if (params->always_on_top) {
        window_flags |= SDL_WINDOW_ALWAYS_ON_TOP;
    }
    if (params->window_borderless) {
        window_flags |= SDL_WINDOW_BORDERLESS;
    }

    screen->window = SDL_CreateWindow(title, x, y, width, height, window_flags);
    if (!screen->window) {
        LOGE("Could not create window: %s", SDL_GetError());
        return false;
    }

    screen->renderer = SDL_CreateRenderer(screen->window, -1, 0);
    if (!screen->renderer) {
        LOGE("Could not create renderer: %s", SDL_GetError());
        goto error_destroy_window;
    }

    SDL_Surface *icon = scrcpy_icon_load();

    if (icon) {
        SDL_SetWindowIcon(screen->window, icon);

        if (SDL_RenderSetLogicalSize(screen->renderer, icon->w, icon->h)) {
            LOGW("Could not set renderer logical size: %s", SDL_GetError());
            // don't fail
        }

        screen->texture = SDL_CreateTextureFromSurface(screen->renderer, icon);
        scrcpy_icon_destroy(icon);
        if (!screen->texture) {
            goto error_destroy_renderer;
        }
    } else {
        screen->texture = NULL;
        LOGW("Could not load icon");
    }

    sc_mouse_capture_init(&screen->mc, screen->window, params->shortcut_mods);

    if (screen->mouse) {
        // Capture mouse on start
        sc_mouse_capture_set_active(&screen->mc, true);
    }

    return true;

error_destroy_window:
    SDL_DestroyWindow(screen->window);
error_destroy_renderer:
    SDL_DestroyRenderer(screen->renderer);

    return false;
}

void
sc_screen_otg_destroy(struct sc_screen_otg *screen) {
#ifdef _WIN32
    if (screen->native) {
        if (screen->hwnd) {
            DestroyWindow(screen->hwnd);
            screen->hwnd = NULL;
        }
        return;
    }
#endif

    if (screen->texture) {
        SDL_DestroyTexture(screen->texture);
    }
    SDL_DestroyRenderer(screen->renderer);
    SDL_DestroyWindow(screen->window);
}

static void
sc_screen_otg_process_key(struct sc_screen_otg *screen,
                             const SDL_KeyboardEvent *event) {
    assert(screen->keyboard);
    struct sc_key_processor *kp = &screen->keyboard->key_processor;

    struct sc_key_event evt = {
        .action = sc_action_from_sdl_keyboard_type(event->type),
        .keycode = sc_keycode_from_sdl(event->keysym.sym),
        .scancode = sc_scancode_from_sdl(event->keysym.scancode),
        .repeat = event->repeat,
        .mods_state = sc_mods_state_from_sdl(event->keysym.mod),
    };

    assert(kp->ops->process_key);
    kp->ops->process_key(kp, &evt, SC_SEQUENCE_INVALID);
}

static void
sc_screen_otg_process_mouse_motion(struct sc_screen_otg *screen,
                                   const SDL_MouseMotionEvent *event) {
    assert(screen->mouse);
    struct sc_mouse_processor *mp = &screen->mouse->mouse_processor;

    struct sc_mouse_motion_event evt = {
        // .position not used for HID events
        .xrel = event->xrel,
        .yrel = event->yrel,
        .buttons_state = sc_mouse_buttons_state_from_sdl(event->state),
    };

    assert(mp->ops->process_mouse_motion);
    mp->ops->process_mouse_motion(mp, &evt);
}

static void
sc_screen_otg_process_mouse_button(struct sc_screen_otg *screen,
                                   const SDL_MouseButtonEvent *event) {
    assert(screen->mouse);
    struct sc_mouse_processor *mp = &screen->mouse->mouse_processor;

    uint32_t sdl_buttons_state = SDL_GetMouseState(NULL, NULL);

    struct sc_mouse_click_event evt = {
        // .position not used for HID events
        .action = sc_action_from_sdl_mousebutton_type(event->type),
        .button = sc_mouse_button_from_sdl(event->button),
        .buttons_state = sc_mouse_buttons_state_from_sdl(sdl_buttons_state),
    };

    assert(mp->ops->process_mouse_click);
    mp->ops->process_mouse_click(mp, &evt);
}

static void
sc_screen_otg_process_mouse_wheel(struct sc_screen_otg *screen,
                                  const SDL_MouseWheelEvent *event) {
    assert(screen->mouse);
    struct sc_mouse_processor *mp = &screen->mouse->mouse_processor;

    uint32_t sdl_buttons_state = SDL_GetMouseState(NULL, NULL);

    struct sc_mouse_scroll_event evt = {
        // .position not used for HID events
#if SDL_VERSION_ATLEAST(2, 0, 18)
        .hscroll = event->preciseX,
        .vscroll = event->preciseY,
#else
        .hscroll = event->x,
        .vscroll = event->y,
#endif
        .hscroll_int = event->x,
        .vscroll_int = event->y,
        .buttons_state = sc_mouse_buttons_state_from_sdl(sdl_buttons_state),
    };

    assert(mp->ops->process_mouse_scroll);
    mp->ops->process_mouse_scroll(mp, &evt);
}

static void
sc_screen_otg_process_gamepad_device(struct sc_screen_otg *screen,
                                     const SDL_ControllerDeviceEvent *event) {
    assert(screen->gamepad);
    struct sc_gamepad_processor *gp = &screen->gamepad->gamepad_processor;

    if (event->type == SDL_CONTROLLERDEVICEADDED) {
        SDL_GameController *gc = SDL_GameControllerOpen(event->which);
        if (!gc) {
            LOGW("Could not open game controller");
            return;
        }

        SDL_Joystick *joystick = SDL_GameControllerGetJoystick(gc);
        if (!joystick) {
            LOGW("Could not get controller joystick");
            SDL_GameControllerClose(gc);
            return;
        }

        struct sc_gamepad_device_event evt = {
            .gamepad_id = SDL_JoystickInstanceID(joystick),
        };
        gp->ops->process_gamepad_added(gp, &evt);
    } else if (event->type == SDL_CONTROLLERDEVICEREMOVED) {
        SDL_JoystickID id = event->which;

        SDL_GameController *gc = SDL_GameControllerFromInstanceID(id);
        if (gc) {
            SDL_GameControllerClose(gc);
        } else {
            LOGW("Unknown gamepad device removed");
        }

        struct sc_gamepad_device_event evt = {
            .gamepad_id = id,
        };
        gp->ops->process_gamepad_removed(gp, &evt);
    }
}

static void
sc_screen_otg_process_gamepad_axis(struct sc_screen_otg *screen,
                                   const SDL_ControllerAxisEvent *event) {
    assert(screen->gamepad);
    struct sc_gamepad_processor *gp = &screen->gamepad->gamepad_processor;

    enum sc_gamepad_axis axis = sc_gamepad_axis_from_sdl(event->axis);
    if (axis == SC_GAMEPAD_AXIS_UNKNOWN) {
        return;
    }

    struct sc_gamepad_axis_event evt = {
        .gamepad_id = event->which,
        .axis = axis,
        .value = event->value,
    };
    gp->ops->process_gamepad_axis(gp, &evt);
}

static void
sc_screen_otg_process_gamepad_button(struct sc_screen_otg *screen,
                                     const SDL_ControllerButtonEvent *event) {
    assert(screen->gamepad);
    struct sc_gamepad_processor *gp = &screen->gamepad->gamepad_processor;

    enum sc_gamepad_button button = sc_gamepad_button_from_sdl(event->button);
    if (button == SC_GAMEPAD_BUTTON_UNKNOWN) {
        return;
    }

    struct sc_gamepad_button_event evt = {
        .gamepad_id = event->which,
        .action = sc_action_from_sdl_controllerbutton_type(event->type),
        .button = button,
    };
    gp->ops->process_gamepad_button(gp, &evt);
}

void
sc_screen_otg_handle_event(struct sc_screen_otg *screen, SDL_Event *event) {
    if (sc_mouse_capture_handle_event(&screen->mc, event)) {
        // The mouse capture handler consumed the event
        return;
    }

    switch (event->type) {
        case SDL_WINDOWEVENT:
            switch (event->window.event) {
                case SDL_WINDOWEVENT_EXPOSED:
                    sc_screen_otg_render(screen);
                    break;
            }
            return;
        case SDL_KEYDOWN:
            if (screen->keyboard) {
                sc_screen_otg_process_key(screen, &event->key);
            }
            break;
        case SDL_KEYUP:
            if (screen->keyboard) {
                sc_screen_otg_process_key(screen, &event->key);
            }
            break;
        case SDL_MOUSEMOTION:
            if (screen->mouse) {
                sc_screen_otg_process_mouse_motion(screen, &event->motion);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (screen->mouse) {
                sc_screen_otg_process_mouse_button(screen, &event->button);
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (screen->mouse) {
                sc_screen_otg_process_mouse_button(screen, &event->button);
            }
            break;
        case SDL_MOUSEWHEEL:
            if (screen->mouse) {
                sc_screen_otg_process_mouse_wheel(screen, &event->wheel);
            }
            break;
        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            // Handle device added or removed even if paused
            if (screen->gamepad) {
                sc_screen_otg_process_gamepad_device(screen, &event->cdevice);
            }
            break;
        case SDL_CONTROLLERAXISMOTION:
            if (screen->gamepad) {
                sc_screen_otg_process_gamepad_axis(screen, &event->caxis);
            }
            break;
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            if (screen->gamepad) {
                sc_screen_otg_process_gamepad_button(screen, &event->cbutton);
            }
            break;
    }
}

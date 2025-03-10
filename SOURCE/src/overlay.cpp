#include "overlay.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include <godot_cpp/classes/input.hpp>

#ifdef _WIN32
#include <windows.h>
#endif

namespace godot {

#ifdef _WIN32
HHOOK Overlay::keyboard_hook = nullptr;
Overlay* Overlay::instance = nullptr; // Initialize static instance pointer
#endif

Overlay::Overlay() {
    printf("Overlay constructor called.\n");

#ifdef _WIN32
    // Set the static instance pointer
    instance = this;

    // Set up a low-level keyboard hook
    keyboard_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(nullptr), 0);
    if (!keyboard_hook) {
        printf("Failed to set up keyboard hook.\n");
    } else {
        printf("Keyboard hook set up successfully.\n");
    }
#endif
}

Overlay::~Overlay() {
#ifdef _WIN32
    // Unhook the keyboard hook
    if (keyboard_hook) {
        UnhookWindowsHookEx(keyboard_hook);
        keyboard_hook = nullptr;
        printf("Keyboard hook unset.\n");
    }

    // Clear the static instance pointer
    instance = nullptr;
#endif

    printf("Overlay destructor called.\n");
}

#ifdef _WIN32
LRESULT CALLBACK Overlay::LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT* pKeyInfo = (KBDLLHOOKSTRUCT*)lParam;

        // Use the static instance pointer to access the Overlay instance
        if (instance) {
            instance->handle_keybind(wParam, pKeyInfo);
        }
    }

    // Pass the event to the next hook in the chain
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void Overlay::handle_keybind(WPARAM wParam, KBDLLHOOKSTRUCT* pKeyInfo) {
    // Check if the Godot window is focused
    if (is_godot_window_focused() || !is_overlay_enabled) {
        printf("Godot window is focused or overlay disabled, ignoring input.\n");
        return; // Ignore input if the Godot window is focused
    }

    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
        // Get the current state of modifier keys
        bool ctrl_pressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000);
        bool shift_pressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000);
        bool alt_pressed = (GetAsyncKeyState(VK_MENU) & 0x8000);

        printf("Key pressed: %d, Ctrl: %d, Shift: %d, Alt: %d\n",
               pKeyInfo->vkCode, ctrl_pressed, shift_pressed, alt_pressed);

        // Check if the pressed key matches the input keybind
        if (input_keybind.is_valid()) {
            int godot_keycode = input_keybind->get_keycode();
            int os_keycode = godot_to_os_keycode(godot_keycode);

            bool key_match = (pKeyInfo->vkCode == os_keycode);
            bool ctrl_match = (ctrl_pressed == input_keybind->is_ctrl_pressed());
            bool shift_match = (shift_pressed == input_keybind->is_shift_pressed());
            bool alt_match = (alt_pressed == input_keybind->is_alt_pressed());

            printf("Input keybind: Godot Key: %d, OS Key: %d, Ctrl: %d, Shift: %d, Alt: %d\n",
                   godot_keycode, os_keycode,
                   input_keybind->is_ctrl_pressed(),
                   input_keybind->is_shift_pressed(),
                   input_keybind->is_alt_pressed());

            if (key_match && ctrl_match && shift_match && alt_match) {
                printf("Input keybind pressed globally.\n");
                if (is_input_passthrough_enabled) {
                    disable_input_passthrough();
                } else {
                    enable_input_passthrough();
                }
            } else {
                printf("Input keybind mismatch.\n");
            }
        }

        // Check if the pressed key matches the visibility keybind
        if (visibility_keybind.is_valid()) {
            int godot_keycode = visibility_keybind->get_keycode();
            int os_keycode = godot_to_os_keycode(godot_keycode);

            bool key_match = (pKeyInfo->vkCode == os_keycode);
            bool ctrl_match = (ctrl_pressed == visibility_keybind->is_ctrl_pressed());
            bool shift_match = (shift_pressed == visibility_keybind->is_shift_pressed());
            bool alt_match = (alt_pressed == visibility_keybind->is_alt_pressed());

            printf("Visibility keybind: Godot Key: %d, OS Key: %d, Ctrl: %d, Shift: %d, Alt: %d\n",
                   godot_keycode, os_keycode,
                   visibility_keybind->is_ctrl_pressed(),
                   visibility_keybind->is_shift_pressed(),
                   visibility_keybind->is_alt_pressed());

            if (key_match && ctrl_match && shift_match && alt_match) {
                printf("Visibility keybind pressed globally.\n");
                if (is_visibility_enabled) {
                    disable_visibility();
                } else {
                    enable_visibility();
                }
            } else {
                printf("Visibility keybind mismatch.\n");
            }
        }
    }
}

// Define the static unordered_map
std::unordered_map<int, int> Overlay::keycode_map = {
    {KEY_ESCAPE, 0x1B},
    {KEY_TAB, 0x09},
    {KEY_ENTER, 0x0D},
    {KEY_BACKSPACE, 0x08},
    {KEY_SPACE, 0x20},
    {KEY_F1, 0x70},
    {KEY_F2, 0x71},
    {KEY_F3, 0x72},
    {KEY_F4, 0x73},
    {KEY_F5, 0x74},
    {KEY_F6, 0x75},
    {KEY_F7, 0x76},
    {KEY_F8, 0x77},
    {KEY_F9, 0x78},
    {KEY_F10, 0x79},
    {KEY_F11, 0x7A},
    {KEY_F12, 0x7B},
    {KEY_A, 0x41},
    {KEY_B, 0x42},
    {KEY_C, 0x43},
    {KEY_D, 0x44},
    {KEY_E, 0x45},
    {KEY_F, 0x46},
    {KEY_G, 0x47},
    {KEY_H, 0x48},
    {KEY_I, 0x49},
    {KEY_J, 0x4A},
    {KEY_K, 0x4B},
    {KEY_L, 0x4C},
    {KEY_M, 0x4D},
    {KEY_N, 0x4E},
    {KEY_O, 0x4F},
    {KEY_P, 0x50},
    {KEY_Q, 0x51},
    {KEY_R, 0x52},
    {KEY_S, 0x53},
    {KEY_T, 0x54},
    {KEY_U, 0x55},
    {KEY_V, 0x56},
    {KEY_W, 0x57},
    {KEY_X, 0x58},
    {KEY_Y, 0x59},
    {KEY_Z, 0x5A},
    {KEY_0, 0x30},
    {KEY_1, 0x31},
    {KEY_2, 0x32},
    {KEY_3, 0x33},
    {KEY_4, 0x34},
    {KEY_5, 0x35},
    {KEY_6, 0x36},
    {KEY_7, 0x37},
    {KEY_8, 0x38},
    {KEY_9, 0x39},
    // Add more mappings as needed
};

// Define the static function to convert Godot keycode to OS keycode
int Overlay::godot_to_os_keycode(int godot_keycode) {
    auto it = keycode_map.find(godot_keycode);
    if (it != keycode_map.end()) {
        return it->second; // Return the mapped OS keycode
    }
    return godot_keycode; // Fallback to the same keycode if not found
}

bool Overlay::is_godot_window_focused() {
    if (!hwnd) {
        return false; // No window handle, assume not focused
    }

    HWND focused_window = GetForegroundWindow();
    return (focused_window == hwnd);
}
#endif

void Overlay::enable_overlay_with_title(const String &title) {
    window_title = title; // Set the window title
    enable_overlay(); // Call the original method
}

void Overlay::enable_overlay() {
    SceneTree *scene_tree = Object::cast_to<SceneTree>(Engine::get_singleton()->get_main_loop());
    if (scene_tree) {
        Window *window = scene_tree->get_root();
        if (window) {
            printf("Godot window is valid.\n");

#ifdef _WIN32
            // Try to get the window ID
            int64_t window_id = window->get_window_id();
            printf("Window ID: %lld\n", window_id);

            // Cast the window ID to HWND
            hwnd = reinterpret_cast<HWND>(static_cast<intptr_t>(window_id));
            if (!hwnd) {
                printf("Failed to cast window ID to HWND. Trying FindWindow...\n");

                // Convert Godot String to C-style string
                CharString title_utf8 = window_title.utf8();
                const char *title_cstr = title_utf8.get_data();

                // Use FindWindowA to get the HWND
                hwnd = FindWindowA(nullptr, title_cstr);
                if (!hwnd) {
                    printf("Failed to find window using FindWindow. Trying GetActiveWindow...\n");

                    // Fallback: Use GetActiveWindow to get the HWND
                    hwnd = GetActiveWindow();
                    if (hwnd) {
                        printf("Window handle (hwnd) found using GetActiveWindow: %p\n", hwnd);
                    } else {
                        printf("Failed to find window using GetActiveWindow. Error: %lu\n", GetLastError());
                        return; // Exit if hwnd is still invalid
                    }
                } else {
                    printf("Window handle (hwnd) found using FindWindow: %p\n", hwnd);
                }
            } else {
                printf("Window handle (hwnd): %p\n", hwnd);
            }

            // Remove title bar and borders
            LONG style = GetWindowLong(hwnd, GWL_STYLE);
            if (SetWindowLong(hwnd, GWL_STYLE, style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU))) {
                printf("Window style updated successfully.\n");
            } else {
                printf("Failed to update window style. Error: %lu\n", GetLastError());
            }

            // Make the window always on top
            if (SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE)) {
                printf("Window set to always on top.\n");
            } else {
                printf("Failed to set window always on top. Error: %lu\n", GetLastError());
            }

            // Enable transparency
            if (SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED)) {
                printf("Transparency enabled.\n");
            } else {
                printf("Failed to enable transparency. Error: %lu\n", GetLastError());
            }

            // Set the background color to pure black (RGB 0, 0, 0)
            hBrush = CreateSolidBrush(RGB(0, 0, 0));
            if (hBrush) {
                SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
                printf("Background brush created and set.\n");
            } else {
                printf("Failed to create background brush. Error: %lu\n", GetLastError());
            }

            // Make the black color transparent
            if (SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY)) {
                printf("Background color set to transparent.\n");
            } else {
                printf("Failed to set background transparency. Error: %lu\n", GetLastError());
            }

            // Make the window ignore input
            if (SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT)) {
                is_input_passthrough_enabled = true;
                printf("Input passthrough enabled.\n");
            } else {
                printf("Failed to enable input passthrough. Error: %lu\n", GetLastError());
            }

            is_overlay_enabled = true;
#endif
        } else {
            printf("Failed to retrieve Godot window.\n");
        }
    } else {
        printf("Failed to retrieve SceneTree.\n");
    }
}

void Overlay::disable_overlay() {
#ifdef _WIN32
        if (hwnd) {
            // Remove the always-on-top flag
            if (SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE)) {
                printf("Window no longer always on top.\n");
            } else {
                printf("Failed to remove always-on-top flag. Error: %lu\n", GetLastError());
            }
    
            // Restore the original window style
            LONG original_style = GetWindowLong(hwnd, GWL_STYLE);
            if (SetWindowLong(hwnd, GWL_STYLE, original_style | WS_OVERLAPPEDWINDOW)) {
                printf("Window style restored.\n");
            } else {
                printf("Failed to restore window style. Error: %lu\n", GetLastError());
            }
    
            // Disable transparency
            if (SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED)) {
                printf("Transparency disabled.\n");
            } else {
                printf("Failed to disable transparency. Error: %lu\n", GetLastError());
            }
    
            // Restore input handling
            if (SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT)) {
                printf("Input passthrough disabled.\n");
            } else {
                printf("Failed to disable input passthrough. Error: %lu\n", GetLastError());
            }

             // Delete the brush to avoid memory leaks
            if (hBrush) {
                DeleteObject(hBrush);
                hBrush = nullptr;
                printf("Background brush deleted.\n");
            }

    
            // Redraw the window to apply changes
            SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    
            is_overlay_enabled = false;
        }
#endif
}

void Overlay::enable_input_passthrough() {
#ifdef _WIN32
    if (hwnd) {
        if (is_overlay_enabled) {
            // Make the window transparent to input
            if (SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_TRANSPARENT)) {
                printf("Input passthrough enabled.\n");
                is_input_passthrough_enabled = true;
            } else {
                printf("Failed to enable input passthrough. Error: %lu\n", GetLastError());
            }
        }
    } else {
        printf("Window handle (hwnd) is invalid. Cannot enable passthrough.\n");
    }
#endif
}

void Overlay::disable_input_passthrough() {
#ifdef _WIN32
    if (hwnd) {
        if (is_overlay_enabled) {
            // Remove the input transparency
            if (SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT)) {
                printf("Input passthrough disabled.\n");
                is_input_passthrough_enabled = false;
                // Focus the window
                if (SetForegroundWindow(hwnd)) {
                    printf("Window focused.\n");
                } else {
                    printf("Failed to focus window. Error: %lu\n", GetLastError());
                }
            } else {
                printf("Failed to disable input passthrough. Error: %lu\n", GetLastError());
            }
        }
    } else {
        printf("Window handle (hwnd) is invalid. Cannot disable passthrough.\n");
    }
#endif
}

void Overlay::enable_visibility() {
#ifdef _WIN32
    if (hwnd) {
        if (is_overlay_enabled) {
            // Make the window fully opaque
            (SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY));
            is_visibility_enabled = true;
            printf("Window shown.\n");
        }
    } else {
        printf("Window handle (hwnd) is invalid. Cannot enable visibility.\n");
    }
#endif
}

void Overlay::disable_visibility() {
#ifdef _WIN32
    if (hwnd) {
        if (is_overlay_enabled) {
            // Make the window fully transparent
            SetLayeredWindowAttributes(hwnd, 0, 0, LWA_ALPHA);
            is_visibility_enabled = false;
            printf("Window hidden.\n");
        }
    } else {
        printf("Window handle (hwnd) is invalid. Cannot disable visibility.\n");
    }
#endif
}

// Keybind methods
void Overlay::set_input_keybind(const Ref<InputEvent> &event) {
    input_keybind = event;
    printf("Input keybind set.\n");
}

Ref<InputEvent> Overlay::get_input_keybind() const {
    return input_keybind;
}

void Overlay::set_visibility_keybind(const Ref<InputEvent> &event) {
    visibility_keybind = event;
    printf("Visibility keybind set.\n");
}

Ref<InputEvent> Overlay::get_visibility_keybind() const {
    return visibility_keybind;
}

// Process method to check for keybind
void Overlay::process(double delta) {
    if (input_keybind.is_valid() && Input::get_singleton()->is_action_just_pressed("overlay_toggle_input")) {
        printf("Input keybind pressed.\n");
        if (is_input_passthrough_enabled) {
            disable_input_passthrough();
        } else {
            enable_input_passthrough();
        }
    }
    if (visibility_keybind.is_valid() && Input::get_singleton()->is_action_just_pressed("overlay_toggle_visibility")) {
        printf("Visibility keybind pressed.\n");
        if (is_visibility_enabled) {
            disable_visibility();
        } else {
            enable_visibility();
        }
    }
}

// Implement the getter methods
bool Overlay::get_is_overlay_enabled() const {
    return is_overlay_enabled;
}

bool Overlay::get_is_input_passthrough_enabled() const {
    return is_input_passthrough_enabled;
}

bool Overlay::get_is_visibility_enabled() const {
    return is_visibility_enabled;
}

// Getter methods for the properties
void Overlay::_bind_methods() {
    printf("Binding methods for Overlay class.\n");

    // Bind methods
    ClassDB::bind_method(D_METHOD("enable_overlay"), &Overlay::enable_overlay);
    ClassDB::bind_method(D_METHOD("disable_overlay"), &Overlay::disable_overlay);
    ClassDB::bind_method(D_METHOD("enable_input_passthrough"), &Overlay::enable_input_passthrough);
    ClassDB::bind_method(D_METHOD("disable_input_passthrough"), &Overlay::disable_input_passthrough);

    // Bind keybind methods
    ClassDB::bind_method(D_METHOD("set_input_keybind", "event"), &Overlay::set_input_keybind);
    ClassDB::bind_method(D_METHOD("get_input_keybind"), &Overlay::get_input_keybind);
    ClassDB::bind_method(D_METHOD("set_visibility_keybind", "event"), &Overlay::set_visibility_keybind);
    ClassDB::bind_method(D_METHOD("get_visibility_keybind"), &Overlay::get_visibility_keybind);

    // Bind visibility methods
    ClassDB::bind_method(D_METHOD("enable_visibility"), &Overlay::enable_visibility);
    ClassDB::bind_method(D_METHOD("disable_visibility"), &Overlay::disable_visibility);

    // Bind getter methods
    ClassDB::bind_method(D_METHOD("get_is_overlay_enabled"), &Overlay::get_is_overlay_enabled);
    ClassDB::bind_method(D_METHOD("get_is_input_passthrough_enabled"), &Overlay::get_is_input_passthrough_enabled);
    ClassDB::bind_method(D_METHOD("get_is_visibility_enabled"), &Overlay::get_is_visibility_enabled);

    // Bind process method
    ClassDB::bind_method(D_METHOD("process", "delta"), &Overlay::process);
}

} // namespace godot
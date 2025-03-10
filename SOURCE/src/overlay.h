#ifndef OVERLAY_H
#define OVERLAY_H

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <unordered_map> // Include the unordered_map header

#ifdef _WIN32
#include <windows.h>
#endif

namespace godot {

class Overlay : public Object {
    GDCLASS(Overlay, Object);

protected:
    static void _bind_methods();

public:
    Overlay();
    ~Overlay();

    void enable_overlay();
    void enable_overlay_with_title(const String &title);
    void disable_overlay();
    void enable_input_passthrough();
    void disable_input_passthrough();

    // Keybind methods
    void set_input_keybind(const Ref<InputEvent> &event);
    Ref<InputEvent> get_input_keybind() const;
    void set_visibility_keybind(const Ref<InputEvent> &event);
    Ref<InputEvent> get_visibility_keybind() const;

    // Visibility methods
    void enable_visibility();
    void disable_visibility();

    // Getter methods for the properties
    bool get_is_overlay_enabled() const;
    bool get_is_input_passthrough_enabled() const;
    bool get_is_visibility_enabled() const;

    // Process method
    void process(double delta);

private:
    bool is_overlay_enabled = false;
    bool is_input_passthrough_enabled = false;
    bool is_visibility_enabled = true;
    String window_title = "Godot";
    Ref<InputEventKey> input_keybind;
    Ref<InputEventKey> visibility_keybind;

#ifdef _WIN32
    HWND hwnd = nullptr;
    HBRUSH hBrush = nullptr;

    // Windows API hook for global input
    static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static HHOOK keyboard_hook;

    // Static unordered_map for keycode mapping
    static std::unordered_map<int, int> keycode_map;

    // Static function to convert Godot keycode to OS keycode
    static int godot_to_os_keycode(int godot_keycode);

    // Static pointer to the Overlay instance
    static Overlay* instance;

    // Keybind handling
    void handle_keybind(WPARAM wParam, KBDLLHOOKSTRUCT* pKeyInfo);

    // Check if the Godot window is focused
    bool is_godot_window_focused();
#endif
};

} // namespace godot

#endif // OVERLAY_H
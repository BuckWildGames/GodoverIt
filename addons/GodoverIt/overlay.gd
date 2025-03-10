extends Node

# Reference to the native Overlay class
var overlay: Overlay

var input_key: Key = KEY_F1
var visibility_key: Key = KEY_F2
var keybinds_found: bool = false

func _ready() -> void:
	set_process(false)
	# Load the input map
	_get_keybinds()
	# Load the native library
	overlay = Overlay.new()
	if overlay:
		_set_keybinds()
		print("Overlay extension loaded successfully")
	else:
		print("Failed to load Overlay extension")


func _get_keybinds() -> void:
	var target_actions = ["overlay_toggle_input", "overlay_toggle_visibility"]
	var actions = InputMap.get_actions()
	var find = [false, false]
	for action in actions:
		if action in target_actions:
			if "input" in action:
				var events = InputMap.action_get_events(action)
				input_key = events[0].get_physical_keycode()
				find[0] = true
			elif "visibility" in action:
				var events = InputMap.action_get_events(action)
				visibility_key = events[0].get_physical_keycode()
				find[1] = true
	if find[0] and find[1]:
		keybinds_found = true
		print("Keybinds found")
	else:
		keybinds_found = false
		print("Keybinds not found")


func _set_keybinds() -> void:
	var key_event = InputEventKey.new()
	key_event.set_keycode(input_key)
	overlay.set_input_keybind(key_event)
	key_event.set_keycode(visibility_key)
	overlay.set_visibility_keybind(key_event)


func _process(delta: float) -> void:
	if not overlay or not keybinds_found:
		set_process(false)
		return
	overlay.process(delta)


func _exit_tree() -> void:
	if overlay:
		disable_overlay()
		overlay.call_deferred("free")


func start_background_thread() -> void:
	if overlay:
		set_process(false)
		overlay.start_background_thread()


func stop_background_thread() -> void:
	if overlay:
		set_process(true)
		overlay.stop_background_thread()


func set_input_keybind(event: InputEvent) -> void:
	if overlay:
		overlay.set_input_keybind(event)


func get_input_keybind() -> InputEvent:
	if not overlay:
		return null
	return overlay.get_input_keybind()


func set_visibility_keybind(event: InputEvent) -> void:
	if overlay:
		overlay.set_visibility_keybind(event)


func get_visibility_keybind() -> InputEvent:
	if not overlay:
		return null
	return overlay.get_visibility_keybind()


func is_overlay_enabled() -> bool:
	if not overlay:
		return false
	return overlay.get_is_overlay_enabled()


func is_input_enabled() -> bool:
	if not overlay:
		return false
	return overlay.get_is_input_passthrough_enabled()


func is_visibility_enabled() -> bool:
	if not overlay:
		return false
	return overlay.get_is_visibility_enabled()


func enable_overlay_with_title(title: String) -> void:
	if overlay:
		overlay.enable_overlay_with_title(title)


func enable_overlay() -> void:
	if overlay:
		overlay.enable_overlay()


func disable_overlay() -> void:
	if overlay:
		overlay.disable_overlay()


func enable_input_passthrough() -> void:
	if overlay:
		overlay.enable_input_passthrough()


func disable_input_passthrough() -> void:
	if overlay:
		overlay.disable_input_passthrough()


func enable_visiblity() -> void:
	if overlay:
		overlay.enable_visibility()


func disable_visiblity() -> void:
	if overlay:
		overlay.disable_visibility()

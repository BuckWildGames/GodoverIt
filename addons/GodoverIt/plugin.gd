@tool
extends EditorPlugin

# Called when the plugin is enabled
func _enter_tree() -> void:
	# Register the Overlay node
	add_custom_type("OverlayNode", "Node", preload("res://addons/GodoverIt/overlay.gd"), null)
	call_deferred("add_input_action", "overlay_toggle_input", KEY_F1)
	call_deferred("add_input_action", "overlay_toggle_visibility", KEY_F2)
	call_deferred("toggle_settings", true)


# Called when the plugin is disabled
func _exit_tree() -> void:
	# Unregister the Overlay node
	remove_custom_type("OverlayNode")
	remove_input_action("overlay_toggle_input")
	remove_input_action("overlay_toggle_visibility")
	toggle_settings(false)


func add_input_action(action_name: String, keycode: Key) -> void:
	var input_map := InputMap
	if input_map.has_action(action_name):
		return
	# Create InputEventKey and set it up
	var event := InputEventKey.new()
	event.set_physical_keycode(keycode)
	# Add the action to the InputMap
	input_map.add_action(action_name)
	input_map.action_add_event(action_name, event)
	# Persist the action in ProjectSettings (for permanent saving)
	var input_path = "input/" + action_name
	var action_data = {
		"deadzone": 0.5,
		"events": [event]
	}
	ProjectSettings.set_setting(input_path, action_data)


func remove_input_action(action_name: String) -> void:
	var input_map := InputMap
	if not input_map.has_action(action_name):
		return
	# Remove the action from the InputMap
	input_map.erase_action(action_name)
	var input_path = "input/" + action_name
	if ProjectSettings.has_setting(input_path):
		ProjectSettings.set_setting(input_path, null)


func toggle_settings(toggled_on: bool) -> void:
	ProjectSettings.set_setting("display/window/size/borderless", toggled_on)
	ProjectSettings.set_setting("display/window/size/always_on_top", toggled_on)
	ProjectSettings.set_setting("display/window/size/transparent", toggled_on)
	ProjectSettings.set_setting("rendering/viewport/transparent_background", toggled_on)
	ProjectSettings.set_setting("display/window/size/no_focus", toggled_on)
	ProjectSettings.set_setting("display/window/per_pixel_transparency/allowed", toggled_on)
	ProjectSettings.save()
	show_confirmation_popup()


func show_confirmation_popup() -> void:
	close_project_settings()
	var dialog = AcceptDialog.new()
	dialog.set_text("Editor Restart Required To Apply Settings.\n(Ignore On Startup)")
	EditorInterface.get_base_control().add_child(dialog)
	dialog.popup_centered()


func close_project_settings() -> void:
	var children = EditorInterface.get_base_control().get_children()
	for child in children:
		if "ProjectSettings" in child.get_name():
			child.hide()
			break

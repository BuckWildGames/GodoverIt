extends UIState

@onready var version_label: Label = $VersionLabel
@onready var label: Label = $Buttons/Label

@export var overlay: Node


func enter(previous : String):
	super.enter(previous)
	#version_label.set_text("Version: " +ConfigManager.get_version())


func button_pressed(button: String) -> void:
	match button:
		"overlay":
			if overlay.is_overlay_enabled():
				overlay.disable_overlay()
				label.set_visible(false)
				print("Disabled Overlay")
			else:
				overlay.enable_overlay()
				label.set_visible(true)
				print("Enabled Overlay")


func _on_quit(quit: String) -> void:
	if quit == "Yes":
		get_tree().quit()

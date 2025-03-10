extends UIState

@onready var version_label: Label = $VersionLabel

@export var overlay: Node


func enter(previous : String):
	super.enter(previous)
	version_label.set_text("Version: " +ConfigManager.get_version())


func button_pressed(button: String) -> void:
	match button:
		"play":
			if overlay.is_overlay_enabled():
				overlay.disable_overlay()
				print("Disabled Overlay")
			else:
				overlay.enable_overlay()
				print("Enabled Overlay")


func _on_quit(quit: String) -> void:
	if quit == "Yes":
		get_tree().quit()

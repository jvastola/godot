@tool
extends Node3D

var _timer := 0.0
var _captured := false

func _ready() -> void:
	if Engine.is_editor_hint():
		# The embedded editor's EditorNode disables the environment on its window
		# (the game window) during init. Restore it after that init so the game
		# strip renders the sky instead of a dark background.
		call_deferred("_restore_game_window_environment")

func _restore_game_window_environment() -> void:
	var game_window := get_tree().root.get_node_or_null("EmbeddedGameWindow")
	if game_window:
		RenderingServer.viewport_set_environment_mode(game_window.get_viewport_rid(), RenderingServer.VIEWPORT_ENVIRONMENT_INHERIT)

func _process(delta: float) -> void:
	var capture := OS.get_cmdline_user_args().has("--capture") or OS.get_cmdline_args().has("--capture")
	if not capture:
		return
	_timer += delta
	if _captured or _timer < 1.0:
		return
	_captured = true
	var root := get_tree().root
	root.get_texture().get_image().save_png("res://capture_editor.png")
	var game_window := root.get_node_or_null("EmbeddedGameWindow")
	if game_window:
		game_window.get_texture().get_image().save_png("res://capture_game.png")
	print("EE-GAME: captured")
	get_tree().quit()

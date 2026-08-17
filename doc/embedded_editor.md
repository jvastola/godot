# Embedded Editor — Design

Feature branch: `godot-editor-3d`

## Goal

Run the real Godot editor with the game scene as the edited scene, and render
that same live scene in a separate game window. Because the editor edits the very
scene the game window shows, adding/placing a node in the editor appears in the
game window immediately (live editing). The editor GUI is never moved or
reparented, so editor teardown and shutdown stay completely normal.

## How it works

New CLI flag `--embed-editor <scene>` (only meaningful with `--editor`). In
`main.cpp` (`Main::start`, after the editor boots):

1. The scene is opened as the editor's edited scene (`EditorNode::load_scene`),
   if it is not already the current edited scene. That scene IS the game.
2. A separate `Window` (`EmbeddedGameWindow`) is created next to the main window.
   It is non-focusable and input-disabled, so it never steals input from the editor.
3. A `Camera3D` (`EmbeddedGameCamera`) is added to that window, positioned to view
   the scene, with a cull mask of layers 1-20 so the editor's gizmo/grid layers
   (24-27) do not render in the game window.

The key mechanism: `scene_root` (the editor's scene-editing `SubViewport`) and the
game `Window` both share the root window's `World3D` by default. The 3D editor
viewport renders the edited scene through that shared world, and so does the game
window's camera. Both view the same live scene tree instance — placing a node in
the editor (scene tree, 3D viewport gizmo, etc.) appears in the game window at
once, and selecting/gizmo interaction is visible in the editor view while the game
window shows only the clean scene.

## Run + verify

```sh
scons target=editor dev_build=yes -j8
./bin/godot.macos.editor.dev.arm64 --editor --path demo/embedded_editor \
  --embed-editor res://main.tscn -- --capture
```

`demo/embedded_editor/main.gd` (a `@tool` script) waits 1s, then saves
`capture_editor.png` (the whole editor window) and `capture_game.png` (the game
window's texture), prints `EE-GAME: captured`, and quits.

The demo scene `main.tscn` is a plain, clear 3D scene (camera, light, a cube) with
no editor machinery in it.

## `@tool` requirement for automation

The editor disables non-tool scripting (`ScriptServer::set_scripting_enabled(false)`,
`editor_node.cpp:8425`), and `GDScript::can_instantiate()` only accepts tool
scripts in that state. The demo's capture-and-quit automation therefore lives in a
`@tool` script. A normal (non-tool) game scene works fine without it — it just has
no automation.

## Why not embed the editor GUI in the scene

An earlier iteration embedded the live `EditorNode` GUI (`gui_base`) inside a
`SubViewport`/`ViewportTexture` placed on a 3D quad in the game scene. It was
rejected:

- The game scene was no longer a clear scene (editor-in-editor-scene-in-editor).
- Reparenting the live editor GUI re-enters the tree and floods the log with
  spurious `Signal 'X' is already connected` errors (widgets connect in
  `NOTIFICATION_ENTER_TREE` without a matching EXIT_TREE disconnect, e.g.
  `node_3d_editor_viewport.cpp`).
- Restoring `gui_base` during shutdown was racy and could double-free with the
  game subtree (`malloc: pointer being freed was not allocated`).

The current design touches none of that: `gui_base` stays `EditorNode`'s child for
the editor's whole lifetime, and the game window is an independent sibling.

## Scope / limitations

- Live rendering relies on the shared `World3D`, so it works for 3D scenes. A 2D
  (canvas) game renders in its own viewport's canvas and is not shown this way.
- The game window uses a fixed camera; it does not follow the scene's own camera.
- Non-tool scripts do not run in the editor process (standard editor behavior);
  the edited scene is displayed statically in both views.

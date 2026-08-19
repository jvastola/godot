# Embedded Editor — Design

Feature branch: `feature/embedded-editor`

## Goal

Run the real Godot editor with the game scene as the edited scene, and render
that same live scene in a separate game window. The editor GUI stays in the
root window (no reparenting). An `EmbeddedEditorScreen` Node3D in the scene
displays the full editor UI as a flat quad above the scene objects — a "2D
viewport in 3D".

## How it works

New CLI flag `--embed-editor <scene>` (only meaningful with `--editor`). In
`main.cpp` (`Main::start`, after the editor boots):

1. The scene is opened as the editor's edited scene (`EditorNode::load_scene`),
   if it is not already the current edited scene. That scene IS the game.
2. A separate `Window` (`EmbeddedGameWindow`) is created at (0,0). It contains
   only a `Camera3D` that renders the live scene — no editor panels.
3. The editor stays in the root window at its default position/size.
4. An `EmbeddedEditorScreen` is added to the edited scene. It creates a
   `MeshInstance3D` with a `QuadMesh` textured from the **root viewport**
   (the editor). The game camera renders this quad in the 3D scene, showing
   the full editor UI on a flat plane above the scene objects.

No circular dependency: the quad's texture comes from the root viewport (the
editor), which is a completely different viewport than the game window viewport
that renders the quad.

## Project setting (Android / headless)

For builds where CLI args are not available (e.g. Android APK exports), set
`editor/embed_editor_scene` in `project.godot`:

```ini
[editor]

embed_editor_scene="res://main.tscn"
```

When this setting is non-empty and `--embed-editor` was not passed on the
command line, the editor activates embedded mode automatically at startup.

## Run + verify

```sh
scons target=editor dev_build=yes -j8
./bin/godot.macos.editor.dev.arm64 --editor --path demo/embedded_editor \
  --embed-editor res://main.tscn -- --capture
```

Or rely on the project setting (no `--embed-editor` needed):

```sh
./bin/godot.macos.editor.dev.arm64 --editor --path demo/embedded_editor -- --capture
```

`demo/embedded_editor/main.gd` (a `@tool` script) waits 1s, then saves
`capture_editor.png` (the whole editor window) and `capture_game.png` (the game
window's texture), prints `EE-GAME: captured`, and quits.

The demo scene `main.tscn` is a plain, clear 3D scene (camera, light, a cube) with
an `EmbeddedEditorScreen` node above the cube.

## Android APK build

To ship the embedded editor in an APK, the build must include editor code.
Standard Godot export templates use `target=template` which strips all editor
classes. Use `target=editor` instead:

```sh
# Build the Android editor template
scons target=editor platform=android arch=arm64

# Or use Godot's own Android editor build pipeline
# (see https://docs.godotengine.org/en/latest/contributing/development/compiling/compiling_for_android.html)
```

The APK will be larger (~50-100 MB more than a template build) because it
includes the full editor (EditorNode, SceneTreeEditor, EditorInspector, etc.).

Set `editor/embed_editor_scene` in `project.godot` so the embedded mode
activates automatically without CLI arguments.

## EmbeddedEditorScreen

A `Node3D` registered as a Godot class (`GDCLASS`). When it enters the scene
tree (`NOTIFICATION_READY`):

1. Gets the root viewport texture (the full editor UI).
2. Creates a `MeshInstance3D` child with a `QuadMesh` (6×4.5 units) and
   `StandardMaterial3D` (unshaded, textured from the root viewport).
3. The quad is a flat plane in the 3D scene showing the live editor.

Add it to any scene in the editor hierarchy or create it via script.

## EmbeddedEditorPanel (fallback)

A lightweight `Control` that shows `SceneTreeEditor` + `EditorInspector` +
toolbar. Used as an alternative when the full editor viewport is not available.
Currently compiled but not used by default in the main flow.

## `@tool` requirement for automation

The editor disables non-tool scripting (`ScriptServer::set_scripting_enabled(false)`,
`editor_node.cpp:8425`), and `GDScript::can_instantiate()` only accepts tool
scripts in that state. The demo's capture-and-quit automation therefore lives in a
`@tool` script. A normal (non-tool) game scene works fine without it — it just has
no automation.

## Scope / limitations

- Live rendering relies on the shared `World3D`, so it works for 3D scenes. A 2D
  (canvas) game renders in its own viewport's canvas and is not shown this way.
- The game window uses a fixed camera; it does not follow the scene's own camera.
- Non-tool scripts do not run in the editor process (standard editor behavior);
  the edited scene is displayed statically in both views.
- Requires `target=editor` builds for APK exports (template builds lack editor
  classes).

/**************************************************************************/
/*  embedded_editor_panel.h                                              */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "scene/gui/panel_container.h"

class Button;
class CreateDialog;
class EditorInspector;
class HSplitContainer;
class SceneTreeEditor;

/**
 * A 2D editor panel meant to be embedded in a game window: the real editor
 * widgets (SceneTreeEditor + EditorInspector) editing the live scene, with a
 * small toolbar for adding nodes and undo/redo. Unlike the full EditorNode, it
 * is a plain Control, so it can be docked inside any window without touching
 * (reparenting) the real editor GUI.
 */
class EmbeddedEditorPanel : public PanelContainer {
private:
	SceneTreeEditor *tree_editor = nullptr;
	EditorInspector *inspector = nullptr;
	CreateDialog *create_dialog = nullptr;
	Button *add_node_button = nullptr;
	Button *undo_button = nullptr;
	Button *redo_button = nullptr;

	void _on_add_node_pressed();
	void _on_create_dialog_confirmed();
	void _on_node_selected();
	void _on_undo_pressed();
	void _on_redo_pressed();
	void _update_buttons();

public:
	EmbeddedEditorPanel();
};

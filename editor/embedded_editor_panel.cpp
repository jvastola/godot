/**************************************************************************/
/*  embedded_editor_panel.cpp                                            */
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

#include "editor/embedded_editor_panel.h"

#include "core/object/class_db.h"
#include "core/object/callable_mp.h"
#include "editor/editor_node.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/gui/create_dialog.h"
#include "editor/inspector/editor_inspector.h"
#include "editor/scene/scene_tree_editor.h"
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/label.h"
#include "scene/gui/split_container.h"
#include "scene/main/node.h"

EmbeddedEditorPanel::EmbeddedEditorPanel() {
	// Dock the panel on the right edge of its window.
	set_anchors_and_offsets_preset(Control::PRESET_RIGHT_WIDE);
	set_offset(SIDE_LEFT, -380);

	VBoxContainer *root_box = memnew(VBoxContainer);
	add_child(root_box);

	Label *title = memnew(Label);
	title->set_text(TTR("Embedded Editor"));
	title->set_theme_type_variation("HeaderSmall");
	root_box->add_child(title);

	HBoxContainer *toolbar = memnew(HBoxContainer);
	root_box->add_child(toolbar);

	add_node_button = memnew(Button);
	add_node_button->set_text(TTR("Add Node"));
	add_node_button->connect("pressed", callable_mp(this, &EmbeddedEditorPanel::_on_add_node_pressed));
	toolbar->add_child(add_node_button);

	undo_button = memnew(Button);
	undo_button->set_text(TTR("Undo"));
	undo_button->connect("pressed", callable_mp(this, &EmbeddedEditorPanel::_on_undo_pressed));
	toolbar->add_child(undo_button);

	redo_button = memnew(Button);
	redo_button->set_text(TTR("Redo"));
	redo_button->connect("pressed", callable_mp(this, &EmbeddedEditorPanel::_on_redo_pressed));
	toolbar->add_child(redo_button);

	HSplitContainer *split = memnew(HSplitContainer);
	split->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	split->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	root_box->add_child(split);

	tree_editor = memnew(SceneTreeEditor);
	tree_editor->set_editor_selection(EditorNode::get_singleton()->get_editor_selection());
	split->add_child(tree_editor);

	inspector = memnew(EditorInspector);
	inspector->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	split->add_child(inspector);

	tree_editor->connect("node_selected", callable_mp(this, &EmbeddedEditorPanel::_on_node_selected));

	create_dialog = memnew(CreateDialog);
	create_dialog->set_base_type("Node");
	create_dialog->connect("create", callable_mp(this, &EmbeddedEditorPanel::_on_create_dialog_confirmed));
	add_child(create_dialog);

	_update_buttons();
}

void EmbeddedEditorPanel::_on_add_node_pressed() {
	create_dialog->popup_create(false);
}

void EmbeddedEditorPanel::_on_create_dialog_confirmed() {
	Node *edited_scene = EditorNode::get_singleton()->get_editor_data().get_edited_scene_root();
	ERR_FAIL_NULL(edited_scene);

	Node *parent = tree_editor->get_selected();
	if (!parent || !(parent == edited_scene || edited_scene->is_ancestor_of(parent))) {
		parent = edited_scene;
	}

	Object *obj = ClassDB::instantiate(create_dialog->get_selected_type());
	Node *node = Object::cast_to<Node>(obj);
	ERR_FAIL_NULL(node);

	node->set_name(create_dialog->get_selected_type());
	node->set_owner(edited_scene);

	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	ur->create_action(TTR("Add Node"));
	ur->add_do_method(parent, "add_child", node, true);
	ur->add_do_reference(node);
	ur->add_undo_method(parent, "remove_child", node);
	ur->commit_action();

	EditorNode::get_singleton()->get_editor_selection()->clear();
	EditorNode::get_singleton()->get_editor_selection()->add_node(node);
}

void EmbeddedEditorPanel::_on_node_selected() {
	inspector->edit(tree_editor->get_selected());
	_update_buttons();
}

void EmbeddedEditorPanel::_on_undo_pressed() {
	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	if (ur->has_undo()) {
		ur->undo();
	}
	_update_buttons();
}

void EmbeddedEditorPanel::_on_redo_pressed() {
	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	if (ur->has_redo()) {
		ur->redo();
	}
	_update_buttons();
}

void EmbeddedEditorPanel::_update_buttons() {
	if (!is_inside_tree()) {
		return;
	}
	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	undo_button->set_disabled(!ur->has_undo());
	redo_button->set_disabled(!ur->has_redo());
}

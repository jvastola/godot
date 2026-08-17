/**************************************************************************/
/*  editor_context.cpp                                                    */
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

#include "editor_context.h"

#include "editor/editor_data.h"
#include "editor/editor_node.h"

EditorContextEditorBridge *EditorContextEditorBridge::get_singleton() {
	static EditorContextEditorBridge singleton;
	return &singleton;
}

Node *EditorContextEditorBridge::get_edited_scene() {
	EditorNode *editor = EditorNode::get_singleton();
	return editor ? editor->get_edited_scene() : nullptr;
}

Ref<Texture2D> EditorContextEditorBridge::get_object_icon(const Object *p_object, const String &p_fallback) {
	EditorNode *editor = EditorNode::get_singleton();
	return editor ? editor->get_object_icon(p_object, p_fallback) : Ref<Texture2D>();
}

Ref<Texture2D> EditorContextEditorBridge::get_class_icon(const String &p_class, const String &p_fallback) {
	EditorNode *editor = EditorNode::get_singleton();
	return editor ? editor->get_class_icon(p_class, p_fallback) : Ref<Texture2D>();
}

bool EditorContextEditorBridge::is_object_of_custom_type(const Object *p_object, const StringName &p_class) {
	EditorNode *editor = EditorNode::get_singleton();
	return editor && editor->is_object_of_custom_type(p_object, p_class);
}

Ref<Script> EditorContextEditorBridge::get_object_custom_type_base(const Object *p_object) {
	EditorNode *editor = EditorNode::get_singleton();
	return editor ? editor->get_object_custom_type_base(p_object) : Ref<Script>();
}

StringName EditorContextEditorBridge::get_object_custom_type_name(const Object *p_object) {
	EditorNode *editor = EditorNode::get_singleton();
	return editor ? editor->get_object_custom_type_name(p_object) : StringName();
}

bool EditorContextEditorBridge::is_exiting() {
	EditorNode *editor = EditorNode::get_singleton();
	return editor && editor->is_exiting();
}

bool EditorContextEditorBridge::is_changing_scene() {
	EditorNode *editor = EditorNode::get_singleton();
	return editor && editor->is_changing_scene();
}

void EditorContextEditorBridge::update_resource_count(Node *p_node, bool p_remove) {
	EditorNode *editor = EditorNode::get_singleton();
	if (editor) {
		editor->update_resource_count(p_node, p_remove);
	}
}

EditorData &EditorContextEditorBridge::get_editor_data() {
	EditorNode *editor = EditorNode::get_singleton();
	if (editor) {
		return editor->get_editor_data();
	}
	static EditorData fallback_editor_data;
	return fallback_editor_data;
}

EditorMainScreen *EditorContextEditorBridge::get_editor_main_screen() {
	EditorNode *editor = EditorNode::get_singleton();
	return editor ? editor->get_editor_main_screen() : nullptr;
}

void EditorContextEditorBridge::hide_unused_editors(Control *p_unused_editor) {
	EditorNode *editor = EditorNode::get_singleton();
	if (editor) {
		editor->hide_unused_editors(p_unused_editor);
	}
}

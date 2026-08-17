/**************************************************************************/
/*  editor_context.h                                                      */
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

#include "core/object/ref_counted.h"

class Control;
class EditorData;
class EditorMainScreen;
class Node;
class Object;
class Script;
class Texture2D;

/**
 * The slice of the editor that editor widgets (EditorInspector, SceneTreeEditor,
 * ...) depend on.
 *
 * The real editor implements it through EditorContextEditorBridge, which forwards
 * to EditorNode::get_singleton().
 */
class EditorContext {
public:
	virtual ~EditorContext() = default;

	virtual Node *get_edited_scene() = 0;

	virtual Ref<Texture2D> get_object_icon(const Object *p_object, const String &p_fallback = "") = 0;
	virtual Ref<Texture2D> get_class_icon(const String &p_class, const String &p_fallback = "") = 0;

	virtual bool is_object_of_custom_type(const Object *p_object, const StringName &p_class) = 0;
	virtual Ref<Script> get_object_custom_type_base(const Object *p_object) = 0;
	virtual StringName get_object_custom_type_name(const Object *p_object) = 0;

	virtual bool is_exiting() = 0;
	virtual bool is_changing_scene() = 0;
	virtual void update_resource_count(Node *p_node, bool p_remove = false) = 0;

	virtual EditorData &get_editor_data() = 0;
	virtual EditorMainScreen *get_editor_main_screen() = 0;

	virtual void hide_unused_editors(Control *p_unused_editor = nullptr) = 0;
};

/**
 * Default EditorContext used inside the real editor process. Forwards every call
 * to EditorNode::get_singleton(), so editor widgets behave exactly as before
 * when no explicit context has been installed.
 */
class EditorContextEditorBridge : public EditorContext {
public:
	static EditorContextEditorBridge *get_singleton();

	virtual Node *get_edited_scene() override;

	virtual Ref<Texture2D> get_object_icon(const Object *p_object, const String &p_fallback = "") override;
	virtual Ref<Texture2D> get_class_icon(const String &p_class, const String &p_fallback = "") override;

	virtual bool is_object_of_custom_type(const Object *p_object, const StringName &p_class) override;
	virtual Ref<Script> get_object_custom_type_base(const Object *p_object) override;
	virtual StringName get_object_custom_type_name(const Object *p_object) override;

	virtual bool is_exiting() override;
	virtual bool is_changing_scene() override;
	virtual void update_resource_count(Node *p_node, bool p_remove = false) override;

	virtual EditorData &get_editor_data() override;
	virtual EditorMainScreen *get_editor_main_screen() override;

	virtual void hide_unused_editors(Control *p_unused_editor = nullptr) override;
};

/**************************************************************************/
/*  embedded_editor_screen.cpp                                            */
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

#include "editor/embedded_editor_screen.h"

#include "core/config/engine.h"
#include "editor/editor_node.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/main/scene_tree.h"
#include "scene/main/viewport.h"
#include "scene/main/window.h"
#include "scene/resources/3d/primitive_meshes.h"
#include "scene/resources/material.h"

void EmbeddedEditorScreen::_bind_methods() {
}

void EmbeddedEditorScreen::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			if (screen_mesh) {
				return;
			}

			// The root window's viewport renders the full editor.  Its texture is a
			// different viewport than the one the game camera writes to, so there is
			// no circular dependency.
			Window *root_win = get_tree()->get_root();
			Viewport *root_vp = root_win;
			// Try editor viewport instead of root (root is XR swapchain on Quest)
			Viewport *editor_vp = nullptr;
			if (Engine::get_singleton()->is_editor_hint() && EditorNode::get_singleton()) {
				Control *gui_base = EditorNode::get_singleton()->get_gui_base();
				if (gui_base) {
					editor_vp = gui_base->get_viewport();
				}
			}
			Viewport *use_vp = editor_vp ? editor_vp : root_vp;
			WARN_PRINT("EmbeddedEditorScreen READY: root_vp texture valid=" + itos(root_vp->get_texture().is_valid()) + " editor_vp=" + itos(editor_vp != nullptr));

			const float quad_w = 3.0;
			const float quad_h = 1.7;

			screen_mesh = memnew(MeshInstance3D);
			screen_mesh->set_name("Screen");
			Ref<QuadMesh> quad;
			quad.instantiate();
			quad->set_size(Size2(quad_w, quad_h));
			Ref<StandardMaterial3D> mat;
			mat.instantiate();
			Ref<Texture2D> tex = use_vp->get_texture();
			WARN_PRINT("EmbeddedEditorScreen tex valid=" + itos(tex.is_valid()) + " tex size=" + (tex.is_valid() ? itos(tex->get_width()) + "x" + itos(tex->get_height()) : String("null")) + " from " + (editor_vp ? String("editor_vp") : String("root_vp")));
			if (tex.is_valid() && tex->get_width() > 0) {
				mat->set_texture(BaseMaterial3D::TEXTURE_ALBEDO, tex);
			} else {
				// Fallback: bright magenta so we know panel is there
				mat->set_albedo(Color(1, 0, 1));
				WARN_PRINT("EmbeddedEditorScreen using fallback magenta");
			}
			mat->set_shading_mode(BaseMaterial3D::SHADING_MODE_UNSHADED);
			mat->set_cull_mode(BaseMaterial3D::CULL_DISABLED);
			// Make it glow so it's visible in dark
			mat->set_feature(BaseMaterial3D::FEATURE_EMISSION, true);
			mat->set_emission(Color(0.2, 0.2, 0.2));
			quad->set_material(mat);
			screen_mesh->set_mesh(quad);
			// Face the player: rotate 180° around Y so front faces -Z (toward origin)
			screen_mesh->set_rotation_degrees(Vector3(0, 180, 0));
			add_child(screen_mesh);
			WARN_PRINT("EmbeddedEditorScreen added quad");

			// Debug cube at same height to verify 3D rendering in editor mode
			MeshInstance3D *cube = memnew(MeshInstance3D);
			cube->set_name("DebugCube");
			Ref<BoxMesh> box;
			box.instantiate();
			box->set_size(Vector3(0.3, 0.3, 0.3));
			cube->set_mesh(box);
			Ref<StandardMaterial3D> cube_mat;
			cube_mat.instantiate();
			cube_mat->set_albedo(Color(1, 0, 0));
			cube_mat->set_shading_mode(BaseMaterial3D::SHADING_MODE_UNSHADED);
			box->set_material(cube_mat);
			cube->set_position(Vector3(0.5, 0, 0));
			screen_mesh->add_child(cube);
		} break;

		default:
			break;
	}
}

EmbeddedEditorScreen::EmbeddedEditorScreen() {
}

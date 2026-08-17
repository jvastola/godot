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

			const float quad_w = 6.0;
			const float quad_h = 4.5;

			screen_mesh = memnew(MeshInstance3D);
			screen_mesh->set_name("Screen");
			Ref<QuadMesh> quad;
			quad.instantiate();
			quad->set_size(Size2(quad_w, quad_h));
			Ref<StandardMaterial3D> mat;
			mat.instantiate();
			mat->set_texture(BaseMaterial3D::TEXTURE_ALBEDO, root_vp->get_texture());
			mat->set_shading_mode(BaseMaterial3D::SHADING_MODE_UNSHADED);
			quad->set_material(mat);
			screen_mesh->set_mesh(quad);
			add_child(screen_mesh);
		} break;

		default:
			break;
	}
}

EmbeddedEditorScreen::EmbeddedEditorScreen() {
}

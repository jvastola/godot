/**************************************************************************/
/*  file_access_android.cpp                                               */
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

#include "file_access_android.h"

#include "thread_jandroid.h"

#include "core/string/print_string.h"
#include "drivers/unix/file_access_unix.h"

#include <android/asset_manager_jni.h>

AAssetManager *FileAccessAndroid::asset_manager = nullptr;
jobject FileAccessAndroid::j_asset_manager = nullptr;

String FileAccessAndroid::get_path() const {
	return path_src;
}

String FileAccessAndroid::get_path_absolute() const {
	return absolute_path;
}

Error FileAccessAndroid::open_internal(const String &p_path, int p_mode_flags) {
	_close();

	path_src = p_path;
	String path = fix_path(p_path).simplify_path();
	absolute_path = path;
	if (path.begins_with("/")) {
		path = path.substr(1);
	} else if (path.begins_with("res://")) {
		path = path.substr(6);
	}

	// Writes and .godot/ editor caches should go to real filesystem, not APK assets.
	// Try APK assets first for reads; fallback to unix filesystem.
	if (p_mode_flags & FileAccess::WRITE) {
		// Delegate writes to unix filesystem (e.g. res://.godot/, user writable paths).
		return unix_file.open_internal(p_path, p_mode_flags);
	}
	asset = AAssetManager_open(asset_manager, path.utf8().get_data(), AASSET_MODE_STREAMING);
	if (asset) {
		len = AAsset_getLength(asset);
		pos = 0;
		eof = false;
		return OK;
	}
	// Not in APK assets - try real filesystem (for .godot caches, writable res:// paths).
	Error err = unix_file.open_internal(p_path, p_mode_flags);
	if (err == OK) {
		using_unix_fallback = true;
		return OK;
	}
	return ERR_CANT_OPEN;
}

void FileAccessAndroid::_close() {
	if (using_unix_fallback) {
		unix_file.close();
		using_unix_fallback = false;
		return;
	}
	if (!asset) {
		return;
	}
	AAsset_close(asset);
	asset = nullptr;
}

bool FileAccessAndroid::is_open() const {
	if (using_unix_fallback) {
		return unix_file.is_open();
	}
	return asset != nullptr;
}

void FileAccessAndroid::seek(uint64_t p_position) {
	if (using_unix_fallback) {
		unix_file.seek(p_position);
		return;
	}
	ERR_FAIL_NULL(asset);

	AAsset_seek(asset, p_position, SEEK_SET);
	pos = p_position;
	if (pos > len) {
		pos = len;
		eof = true;
	} else {
		eof = false;
	}
}

void FileAccessAndroid::seek_end(int64_t p_position) {
	if (using_unix_fallback) {
		unix_file.seek_end(p_position);
		return;
	}
	ERR_FAIL_NULL(asset);
	AAsset_seek(asset, p_position, SEEK_END);
	pos = len + p_position;
}

uint64_t FileAccessAndroid::get_position() const {
	if (using_unix_fallback) {
		return unix_file.get_position();
	}
	return pos;
}

uint64_t FileAccessAndroid::get_length() const {
	if (using_unix_fallback) {
		return unix_file.get_length();
	}
	return len;
}

bool FileAccessAndroid::eof_reached() const {
	if (using_unix_fallback) {
		return unix_file.eof_reached();
	}
	return eof;
}

uint64_t FileAccessAndroid::get_buffer(uint8_t *p_dst, uint64_t p_length) const {
	if (using_unix_fallback) {
		return unix_file.get_buffer(p_dst, p_length);
	}
	ERR_FAIL_COND_V(!p_dst && p_length > 0, -1);

	int r = AAsset_read(asset, p_dst, p_length);

	if (pos + p_length > len) {
		eof = true;
	}

	if (r >= 0) {
		pos += r;
		if (pos > len) {
			pos = len;
		}
	}

	return r;
}

int64_t FileAccessAndroid::_get_size(const String &p_file) {
	// p_file is a resource path - try APK assets first, then filesystem fallback.
	String path = fix_path(p_file).simplify_path();
	if (path.begins_with("/")) {
		path = path.substr(1);
	} else if (path.begins_with("res://")) {
		path = path.substr(6);
	}
	AAsset *at = AAssetManager_open(asset_manager, path.utf8().get_data(), AASSET_MODE_STREAMING);
	if (at) {
		int64_t s = AAsset_getLength64(at);
		AAsset_close(at);
		return s;
	}
	return unix_file._get_size(p_file);
}

Error FileAccessAndroid::get_error() const {
	if (using_unix_fallback) {
		return unix_file.get_error();
	}
	return eof ? ERR_FILE_EOF : OK; // not sure what else it may happen
}

void FileAccessAndroid::flush() {
	if (using_unix_fallback) {
		unix_file.flush();
		return;
	}
	ERR_FAIL();
}

bool FileAccessAndroid::store_buffer(const uint8_t *p_src, uint64_t p_length) {
	if (using_unix_fallback) {
		return unix_file.store_buffer(p_src, p_length);
	}
	ERR_FAIL_V(false);
}

bool FileAccessAndroid::file_exists(const String &p_path) {
	String path = fix_path(p_path).simplify_path();
	if (path.begins_with("/")) {
		path = path.substr(1);
	} else if (path.begins_with("res://")) {
		path = path.substr(6);
	}

	AAsset *at = AAssetManager_open(asset_manager, path.utf8().get_data(), AASSET_MODE_STREAMING);

	if (at) {
		AAsset_close(at);
		return true;
	}
	// Fallback to filesystem for .godot caches etc.
	return unix_file.file_exists(p_path);
}

void FileAccessAndroid::close() {
	_close();
}

FileAccessAndroid::~FileAccessAndroid() {
	_close();
}

void FileAccessAndroid::setup(jobject p_asset_manager) {
	JNIEnv *env = get_jni_env();
	j_asset_manager = env->NewGlobalRef(p_asset_manager);
	asset_manager = AAssetManager_fromJava(env, j_asset_manager);
}

void FileAccessAndroid::terminate() {
	JNIEnv *env = get_jni_env();
	ERR_FAIL_NULL(env);

	env->DeleteGlobalRef(j_asset_manager);
}

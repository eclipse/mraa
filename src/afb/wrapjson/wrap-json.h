/*
 Copyright (C) 2016, 2017 "IoT.bzh"

 author: José Bollo <jose.bollo@iot.bzh>

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#pragma once

#include <stdarg.h>
#include <json-c/json.h>

extern int wrap_json_get_error_position(int rc);
extern int wrap_json_get_error_code(int rc);
extern const char *wrap_json_get_error_string(int rc);

extern int wrap_json_vpack(struct json_object **result, const char *desc, va_list args);
extern int wrap_json_pack(struct json_object **result, const char *desc, ...);

extern int wrap_json_vunpack(struct json_object *object, const char *desc, va_list args);
extern int wrap_json_unpack(struct json_object *object, const char *desc, ...);
extern int wrap_json_vcheck(struct json_object *object, const char *desc, va_list args);
extern int wrap_json_check(struct json_object *object, const char *desc, ...);
extern int wrap_json_vmatch(struct json_object *object, const char *desc, va_list args);
extern int wrap_json_match(struct json_object *object, const char *desc, ...);

extern void wrap_json_optarray_for_all(struct json_object *object, void (*callback)(void*,struct json_object*), void *closure);
extern void wrap_json_array_for_all(struct json_object *object, void (*callback)(void*,struct json_object*), void *closure);

extern void wrap_json_optarray_for_all(struct json_object *object, void (*callback)(void*,struct json_object*), void *closure);
extern void wrap_json_array_for_all(struct json_object *object, void (*callback)(void*,struct json_object*), void *closure);
extern void wrap_json_object_for_all(struct json_object *object, void (*callback)(void*,struct json_object*,const char*), void *closure);
extern void wrap_json_optobject_for_all(struct json_object *object, void (*callback)(void*,struct json_object*,const char*), void *closure);
extern void wrap_json_for_all(struct json_object *object, void (*callback)(void*,struct json_object*,const char*), void *closure);


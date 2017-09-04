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

#include <string.h>

#include "wrap-json.h"

#define STACKCOUNT  32
#define STRCOUNT    8

enum {
	wrap_json_error_none,
	wrap_json_error_null_object,
	wrap_json_error_truncated,
	wrap_json_error_internal_error,
	wrap_json_error_out_of_memory,
	wrap_json_error_invalid_character,
	wrap_json_error_too_long,
	wrap_json_error_too_deep,
	wrap_json_error_null_spec,
	wrap_json_error_null_key,
	wrap_json_error_null_string,
	wrap_json_error_out_of_range,
	wrap_json_error_incomplete,
	wrap_json_error_missfit_type,
	wrap_json_error_key_not_found,
	_wrap_json_error_count_
};

static const char ignore_all[] = " \t\n\r,:";
static const char pack_accept_arr[] = "][{snbiIfoO";
static const char pack_accept_key[] = "s}";
#define pack_accept_any (&pack_accept_arr[1])

static const char unpack_accept_arr[] = "*!][{snbiIfFoO";
static const char unpack_accept_key[] = "*!s}";
#define unpack_accept_any (&unpack_accept_arr[3])

static const char *pack_errors[_wrap_json_error_count_] =
{
	[wrap_json_error_none] = "unknown error",
	[wrap_json_error_null_object] = "null object",
	[wrap_json_error_truncated] = "truncated",
	[wrap_json_error_internal_error] = "internal error",
	[wrap_json_error_out_of_memory] = "out of memory",
	[wrap_json_error_invalid_character] = "invalid character",
	[wrap_json_error_too_long] = "too long",
	[wrap_json_error_too_deep] = "too deep",
	[wrap_json_error_null_spec] = "spec is NULL",
	[wrap_json_error_null_key] = "key is NULL",
	[wrap_json_error_null_string] = "string is NULL",
	[wrap_json_error_out_of_range] = "array too small",
	[wrap_json_error_incomplete] = "incomplete container",
	[wrap_json_error_missfit_type] = "missfit of type",
	[wrap_json_error_key_not_found] = "key not found"
};

int wrap_json_get_error_position(int rc)
{
	if (rc < 0)
		rc = -rc;
	return (rc >> 4) + 1;
}

int wrap_json_get_error_code(int rc)
{
	if (rc < 0)
		rc = -rc;
	return rc & 15;
}

const char *wrap_json_get_error_string(int rc)
{
	rc = wrap_json_get_error_code(rc);
	if (rc >= sizeof pack_errors / sizeof *pack_errors)
		rc = 0;
	return pack_errors[rc];
}



static inline const char *skip(const char *d)
{
	while (*d && strchr(ignore_all, *d))
		d++;
	return d;
}

int wrap_json_vpack(struct json_object **result, const char *desc, va_list args)
{
	/* TODO: the case of structs with key being single char should be optimized */
	int nstr, notnull, nullable, rc;
	size_t sz, dsz, ssz;
	char *s;
	char c;
	const char *d;
	char buffer[256];
	struct { const char *str; size_t sz; } strs[STRCOUNT];
	struct { struct json_object *cont, *key; const char *acc; char type; } stack[STACKCOUNT], *top;
	struct json_object *obj;

	ssz = sizeof buffer;
	s = buffer;
	top = stack;
	top->key = NULL;
	top->cont = NULL;
	top->acc = pack_accept_any;
	top->type = 0;
	d = desc;
	if (!d)
		goto null_spec;
	d = skip(d);
	for(;;) {
		c = *d;
		if (!c)
			goto truncated;
		if (!strchr(top->acc, c))
			goto invalid_character;
		d = skip(++d);
		switch(c) {
		case 's':
			nullable = 0;
			notnull = 0;
			nstr = 0;
			sz = 0;
			for (;;) {
				strs[nstr].str = va_arg(args, const char*);
				if (strs[nstr].str)
					notnull = 1;
				if (*d == '?') {
					d = skip(++d);
					nullable = 1;
				}
				switch(*d) {
				case '%': strs[nstr].sz = va_arg(args, size_t); d = skip(++d); break;
				case '#': strs[nstr].sz = (size_t)va_arg(args, int); d = skip(++d); break;
				default: strs[nstr].sz = strs[nstr].str ? strlen(strs[nstr].str) : 0; break;
				}
				sz += strs[nstr++].sz;
				if (*d == '?') {
					d = skip(++d);
					nullable = 1;
				}
				if (*d != '+')
					break;
				if (nstr >= STRCOUNT)
					goto too_long;
				d = skip(++d);
			}
			if (*d == '*')
				nullable = 1;
			if (notnull) {
				if (sz > ssz) {
					ssz += ssz;
					if (ssz < sz)
						ssz = sz;
					s = alloca(sz);
				}
				dsz = sz;
				while (nstr) {
					nstr--;
					dsz -= strs[nstr].sz;
					memcpy(&s[dsz], strs[nstr].str, strs[nstr].sz);
				}
				obj = json_object_new_string_len(s, (int)sz);
				if (!obj)
					goto out_of_memory;
			} else if (nullable)
				obj = NULL;
			else
				goto null_string;
			break;
		case 'n':
			obj = NULL;
			break;
		case 'b':
			obj = json_object_new_boolean(va_arg(args, int));
			if (!obj)
				goto out_of_memory;
			break;
		case 'i':
			obj = json_object_new_int(va_arg(args, int));
			if (!obj)
				goto out_of_memory;
			break;
		case 'I':
			obj = json_object_new_int64(va_arg(args, int64_t));
			if (!obj)
				goto out_of_memory;
			break;
		case 'f':
			obj = json_object_new_double(va_arg(args, double));
			if (!obj)
				goto out_of_memory;
			break;
		case 'o':
		case 'O':
			obj = va_arg(args, struct json_object*);
			if (*d == '?')
				d = skip(++d);
			else if (*d != '*' && !obj)
				goto null_object;
			if (c == 'O')
				json_object_get(obj);
			break;
		case '[':
		case '{':
			if (++top >= &stack[STACKCOUNT])
				goto too_deep;
			top->key = NULL;
			if (c == '[') {
				top->type = ']';
				top->acc = pack_accept_arr;
				top->cont = json_object_new_array();
			} else {
				top->type = '}';
				top->acc = pack_accept_key;
				top->cont = json_object_new_object();
			}
			if (!top->cont)
				goto out_of_memory;
			continue;
		case '}':
		case ']':
			if (c != top->type || top <= stack)
				goto internal_error;
			obj = (top--)->cont;
			if (*d == '*' && !(c == '}' ? json_object_object_length(obj) : json_object_array_length(obj))) {
				json_object_put(obj);
				obj = NULL;
			}
			break;
		default:
			goto internal_error;
		}
		switch (top->type) {
		case 0:
			if (top != stack)
				goto internal_error;
			if (*d)
				goto invalid_character;
			*result = obj;
			return 0;
		case ']':
			if (obj || *d != '*')
				json_object_array_add(top->cont, obj);
			if (*d == '*')
				d = skip(++d);
			break;
		case '}':
			if (!obj)
				goto null_key;
			top->key = obj;
			top->acc = pack_accept_any;
			top->type = ':';
			break;
		case ':':
			if (obj || *d != '*')
				json_object_object_add(top->cont, json_object_get_string(top->key), obj);
			if (*d == '*')
				d = skip(++d);
			json_object_put(top->key);
			top->key = NULL;
			top->acc = pack_accept_key;
			top->type = '}';
			break;
		default:
			goto internal_error;
		}
	}

null_object:
	rc = wrap_json_error_null_object;
	goto error;
truncated:
	rc = wrap_json_error_truncated;
	goto error;
internal_error:
	rc = wrap_json_error_internal_error;
	goto error;
out_of_memory:
	rc = wrap_json_error_out_of_memory;
	goto error;
invalid_character:
	rc = wrap_json_error_invalid_character;
	goto error;
too_long:
	rc = wrap_json_error_too_long;
	goto error;
too_deep:
	rc = wrap_json_error_too_deep;
	goto error;
null_spec:
	rc = wrap_json_error_null_spec;
	goto error;
null_key:
	rc = wrap_json_error_null_key;
	goto error;
null_string:
	rc = wrap_json_error_null_string;
	goto error;
error:
	do {
		json_object_put(top->key);
		json_object_put(top->cont);
	} while (--top >= stack);
	*result = NULL;
	rc = rc | (int)((d - desc) << 4);
	return -rc;
}

int wrap_json_pack(struct json_object **result, const char *desc, ...)
{
	int rc;
	va_list args;

	va_start(args, desc);
	rc = wrap_json_vpack(result, desc, args);
	va_end(args);
	return rc;
}

static int vunpack(struct json_object *object, const char *desc, va_list args, int store)
{
	int rc = 0, optionnal, ignore;
	char c, xacc[2] = { 0, 0 };
	const char *acc;
	const char *d, *fit = NULL;
	const char *key = NULL;
	const char **ps = NULL;
	double *pf = NULL;
	int *pi = NULL;
	int64_t *pI = NULL;
	size_t *pz = NULL;
	struct { struct json_object *parent; const char *acc; int index, count; char type; } stack[STACKCOUNT], *top;
	struct json_object *obj;
	struct json_object **po;

	xacc[0] = 0;
	ignore = 0;
	top = NULL;
	acc = unpack_accept_any;
	d = desc;
	if (!d)
		goto null_spec;
	d = skip(d);
	obj = object;
	for(;;) {
		fit = d;
		c = *d;
		if (!c)
			goto truncated;
		if (!strchr(acc, c))
			goto invalid_character;
		d = skip(++d);
		switch(c) {
		case 's':
			if (xacc[0] == '}') {
				/* expects a key */
				key = va_arg(args, const char *);
				if (!key)
					goto null_key;
				if (*d != '?')
					optionnal = 0;
				else {
					optionnal = 1;
					d = skip(++d);
				}
				if (ignore)
					ignore++;
				else {
					if (json_object_object_get_ex(top->parent, key, &obj)) {
						/* found */
						top->index++;
					} else {
						/* not found */
						if (!optionnal)
							goto key_not_found;
						ignore = 1;
						obj = NULL;
					}
				}
				xacc[0] = ':';
				acc = unpack_accept_any;
				continue;
			}
			/* get a string */
			if (store)
				ps = va_arg(args, const char **);
			if (!ignore) {
				if (!json_object_is_type(obj, json_type_string))
					goto missfit;
				if (store && ps)
					*ps = json_object_get_string(obj);
			}
			if (*d == '%') {
				d = skip(++d);
				if (store) {
					pz = va_arg(args, size_t *);
					if (!ignore && pz)
						*pz = (size_t)json_object_get_string_len(obj);
				}
			}
			break;
		case 'n':
			if (!ignore && !json_object_is_type(obj, json_type_null))
				goto missfit;
			break;
		case 'b':
			if (store)
				pi = va_arg(args, int *);

			if (!ignore) {
				if (!json_object_is_type(obj, json_type_boolean))
					goto missfit;
				if (store && pi)
					*pi = json_object_get_boolean(obj);
			}
			break;
		case 'i':
			if (store)
				pi = va_arg(args, int *);

			if (!ignore) {
				if (!json_object_is_type(obj, json_type_int))
					goto missfit;
				if (store && pi)
					*pi = json_object_get_int(obj);
			}
			break;
		case 'I':
			if (store)
				pI = va_arg(args, int64_t *);

			if (!ignore) {
				if (!json_object_is_type(obj, json_type_int))
					goto missfit;
				if (store && pI)
					*pI = json_object_get_int64(obj);
			}
			break;
		case 'f':
		case 'F':
			if (store)
				pf = va_arg(args, double *);

			if (!ignore) {
				if (!(json_object_is_type(obj, json_type_double) || (c == 'F' && json_object_is_type(obj, json_type_int))))
					goto missfit;
				if (store && pf)
					*pf = json_object_get_double(obj);
			}
			break;
		case 'o':
		case 'O':
			if (store) {
				po = va_arg(args, struct json_object **);
				if (!ignore && po) {
					if (c == 'O')
						obj = json_object_get(obj);
					*po = obj;
				}
			}
			break;

		case '[':
		case '{':
			if (!top)
				top = stack;
			else if (++top  >= &stack[STACKCOUNT])
				goto too_deep;

			top->acc = acc;
			top->type = xacc[0];
			top->index = 0;
			top->parent = obj;
			if (ignore)
				ignore++;
			if (c == '[') {
				if (!ignore) {
					if (!json_object_is_type(obj, json_type_array))
						goto missfit;
					top->count = json_object_array_length(obj);
				}
				xacc[0] = ']';
				acc = unpack_accept_arr;
			} else {
				if (!ignore) {
					if (!json_object_is_type(obj, json_type_object))
						goto missfit;
					top->count = json_object_object_length(obj);
				}
				xacc[0] = '}';
				acc = unpack_accept_key;
				continue;
			}
			break;
		case '}':
		case ']':
			if (!top || c != xacc[0])
				goto internal_error;
			acc = top->acc;
			xacc[0] = top->type;
			top = top == stack ? NULL : top - 1;
			if (ignore)
				ignore--;
			break;
		case '!':
			if (*d != xacc[0])
				goto invalid_character;
			if (!ignore && top->index != top->count)
				goto incomplete;
			/*@fallthrough@*/
		case '*':
			acc = xacc;
			continue;
		default:
			goto internal_error;
		}
		switch (xacc[0]) {
		case 0:
			if (top)
				goto internal_error;
			if (*d)
				goto invalid_character;
			return 0;
		case ']':
			if (!ignore) {
				key = strchr(unpack_accept_arr, *d);
				if (key && key >= unpack_accept_any) {
					if (top->index >= top->count)
						goto out_of_range;
					obj = json_object_array_get_idx(top->parent, top->index++);
				}
			}
			break;
		case ':':
			acc = unpack_accept_key;
			xacc[0] = '}';
			if (ignore)
				ignore--;
			break;
		default:
			goto internal_error;
		}
	}
truncated:
	rc = wrap_json_error_truncated;
	goto error;
internal_error:
	rc = wrap_json_error_internal_error;
	goto error;
invalid_character:
	rc = wrap_json_error_invalid_character;
	goto error;
too_deep:
	rc = wrap_json_error_too_deep;
	goto error;
null_spec:
	rc = wrap_json_error_null_spec;
	goto error;
null_key:
	rc = wrap_json_error_null_key;
	goto error;
out_of_range:
	rc = wrap_json_error_out_of_range;
	goto error;
incomplete:
	rc = wrap_json_error_incomplete;
	goto error;
missfit:
	rc = wrap_json_error_missfit_type;
	goto errorfit;
key_not_found:
	rc = wrap_json_error_key_not_found;
	goto error;
errorfit:
	d = fit;
error:
	rc = rc | (int)((d - desc) << 4);
	return -rc;
}

int wrap_json_vcheck(struct json_object *object, const char *desc, va_list args)
{
	return vunpack(object, desc, args, 0);
}

int wrap_json_check(struct json_object *object, const char *desc, ...)
{
	int rc;
	va_list args;

	va_start(args, desc);
	rc = vunpack(object, desc, args, 0);
	va_end(args);
	return rc;
}

int wrap_json_vmatch(struct json_object *object, const char *desc, va_list args)
{
	return !vunpack(object, desc, args, 0);
}

int wrap_json_match(struct json_object *object, const char *desc, ...)
{
	int rc;
	va_list args;

	va_start(args, desc);
	rc = vunpack(object, desc, args, 0);
	va_end(args);
	return !rc;
}

int wrap_json_vunpack(struct json_object *object, const char *desc, va_list args)
{
	return vunpack(object, desc, args, 1);
}

int wrap_json_unpack(struct json_object *object, const char *desc, ...)
{
	int rc;
	va_list args;

	va_start(args, desc);
	rc = vunpack(object, desc, args, 1);
	va_end(args);
	return rc;
}

static void object_for_all(struct json_object *object, void (*callback)(void*,struct json_object*,const char*), void *closure)
{
	struct json_object_iterator it = json_object_iter_begin(object);
	struct json_object_iterator end = json_object_iter_end(object);
	while (!json_object_iter_equal(&it, &end)) {
		callback(closure, json_object_iter_peek_value(&it), json_object_iter_peek_name(&it));
		json_object_iter_next(&it);
	}
}

static void array_for_all(struct json_object *object, void (*callback)(void*,struct json_object*), void *closure)
{
	int n = json_object_array_length(object);
	int i = 0;
	while(i < n)
		callback(closure, json_object_array_get_idx(object, i++));
}

void wrap_json_optarray_for_all(struct json_object *object, void (*callback)(void*,struct json_object*), void *closure)
{
	if (json_object_is_type(object, json_type_array))
		array_for_all(object, callback, closure);
	else
		callback(closure, object);
}

void wrap_json_array_for_all(struct json_object *object, void (*callback)(void*,struct json_object*), void *closure)
{
	if (json_object_is_type(object, json_type_array))
		array_for_all(object, callback, closure);
}

void wrap_json_object_for_all(struct json_object *object, void (*callback)(void*,struct json_object*,const char*), void *closure)
{
	if (json_object_is_type(object, json_type_object))
		object_for_all(object, callback, closure);
}

void wrap_json_optobject_for_all(struct json_object *object, void (*callback)(void*,struct json_object*,const char*), void *closure)
{
	if (json_object_is_type(object, json_type_object))
		object_for_all(object, callback, closure);
	else
		callback(closure, object, NULL);
}

void wrap_json_for_all(struct json_object *object, void (*callback)(void*,struct json_object*,const char*), void *closure)
{
	if (!object)
		/* do nothing */;
	else if (json_object_is_type(object, json_type_object))
		object_for_all(object, callback, closure);
	else if (!json_object_is_type(object, json_type_array))
		callback(closure, object, NULL);
	else {
		int n = json_object_array_length(object);
		int i = 0;
		while(i < n)
			callback(closure, json_object_array_get_idx(object, i++), NULL);
	}
}

#if defined(WRAP_JSON_TEST)
#include <stdio.h>

void p(const char *desc, ...)
{
	int rc;
	va_list args;
	struct json_object *result;

	va_start(args, desc);
	rc = wrap_json_vpack(&result, desc, args);
	va_end(args);
	if (!rc) 
		printf("  SUCCESS %s\n\n", json_object_to_json_string(result));
	else
		printf("  ERROR[char %d err %d] %s\n\n", wrap_json_get_error_position(rc), wrap_json_get_error_code(rc), wrap_json_get_error_string(rc));
	json_object_put(result);
}

const char *xs[10];
int *xi[10];
int64_t *xI[10];
double *xf[10];
struct json_object *xo[10];
size_t xz[10];

void u(const char *value, const char *desc, ...)
{
	unsigned m, k;
	int rc;
	va_list args;
	struct json_object *obj, *o;

	memset(xs, 0, sizeof xs);
	memset(xi, 0, sizeof xi);
	memset(xI, 0, sizeof xI);
	memset(xf, 0, sizeof xf);
	memset(xo, 0, sizeof xo);
	memset(xz, 0, sizeof xz);
	obj = json_tokener_parse(value);
	va_start(args, desc);
	rc = wrap_json_vunpack(obj, desc, args);
	va_end(args);
	if (rc) 
		printf("  ERROR[char %d err %d] %s\n\n", wrap_json_get_error_position(rc), wrap_json_get_error_code(rc), wrap_json_get_error_string(rc));
	else {
		value = NULL;
		printf("  SUCCESS");
		va_start(args, desc);
		k = m = 0;
		while(*desc) {
			switch(*desc) {
			case '{': m = (m << 1) | 1; k = 1; break;
			case '}': m = m >> 1; k = m&1; break;
			case '[': m = m << 1; k = 0; break;
			case ']': m = m >> 1; k = m&1; break;
			case 's': printf(" s:%s", k ? va_arg(args, const char*) : *(va_arg(args, const char**)?:&value)); k ^= m&1; break;
			case '%': printf(" %%:%zu", *va_arg(args, size_t*)); k = m&1; break;
			case 'n': printf(" n"); k = m&1; break;
			case 'b': printf(" b:%d", *va_arg(args, int*)); k = m&1; break;
			case 'i': printf(" i:%d", *va_arg(args, int*)); k = m&1; break;
			case 'I': printf(" I:%lld", *va_arg(args, int64_t*)); k = m&1; break;
			case 'f': printf(" f:%f", *va_arg(args, double*)); k = m&1; break;
			case 'F': printf(" F:%f", *va_arg(args, double*)); k = m&1; break;
			case 'o': printf(" o:%s", json_object_to_json_string(*va_arg(args, struct json_object**))); k = m&1; break;
			case 'O': o = *va_arg(args, struct json_object**); printf(" O:%s", json_object_to_json_string(o)); json_object_put(o); k = m&1; break;
			default: break;
			}
			desc++;
		}
		va_end(args);
		printf("\n\n");
	}
	json_object_put(obj);
}

#define P(...) do{ printf("pack(%s)\n",#__VA_ARGS__); p(__VA_ARGS__); } while(0)
#define U(...) do{ printf("unpack(%s)\n",#__VA_ARGS__); u(__VA_ARGS__); } while(0)

int main()
{
	char buffer[4] = {'t', 'e', 's', 't'};

	P("n");
	P("b", 1);
	P("b", 0);
	P("i", 1);
	P("I", (uint64_t)0x123456789abcdef);
	P("f", 3.14);
	P("s", "test");
	P("s?", "test");
	P("s?", NULL);
	P("s#", "test asdf", 4);
	P("s%", "test asdf", (size_t)4);
	P("s#", buffer, 4);
	P("s%", buffer, (size_t)4);
	P("s++", "te", "st", "ing");
	P("s#+#+", "test", 1, "test", 2, "test");
	P("s%+%+", "test", (size_t)1, "test", (size_t)2, "test");
	P("{}", 1.0);
	P("[]", 1.0);
	P("o", json_object_new_int(1));
	P("o?", json_object_new_int(1));
	P("o?", NULL);
	P("O", json_object_new_int(1));
	P("O?", json_object_new_int(1));
	P("O?", NULL);
	P("{s:[]}", "foo");
	P("{s+#+: []}", "foo", "barbar", 3, "baz");
	P("{s:s,s:o,s:O}", "a", NULL, "b", NULL, "c", NULL);
	P("{s:**}", "a", NULL);
	P("{s:s*,s:o*,s:O*}", "a", NULL, "b", NULL, "c", NULL);
	P("[i,i,i]", 0, 1, 2);
	P("[s,o,O]", NULL, NULL, NULL);
	P("[**]", NULL);
	P("[s*,o*,O*]", NULL, NULL, NULL);
	P(" s ", "test");
	P("[ ]");
	P("[ i , i,  i ] ", 1, 2, 3);
	P("{\n\n1");
	P("[}");
	P("{]");
	P("[");
	P("{");
	P("[i]a", 42);
	P("ia", 42);
	P("s", NULL);
	P("+", NULL);
	P(NULL);
	P("{s:i}", NULL, 1);
	P("{ {}: s }", "foo");
	P("{ s: {},  s:[ii{} }", "foo", "bar", 12, 13);
	P("[[[[[   [[[[[  [[[[ }]]]] ]]]] ]]]]]");

	U("true", "b", &xi[0]);
	U("false", "b", &xi[0]);
	U("null", "n");
	U("42", "i", &xi[0]);
	U("123456789", "I", &xI[0]);
	U("3.14", "f", &xf[0]);
	U("12345", "F", &xf[0]);
	U("3.14", "F", &xf[0]);
	U("\"foo\"", "s", &xs[0]);
	U("\"foo\"", "s%", &xs[0], &xz[0]);
	U("{}", "{}");
	U("[]", "[]");
	U("{}", "o", &xo[0]);
	U("{}", "O", &xo[0]);
	U("{\"foo\":42}", "{si}", "foo", &xi[0]);
	U("[1,2,3]", "[i,i,i]", &xi[0], &xi[1], &xi[2]);
	U("{\"a\":1,\"b\":2,\"c\":3}", "{s:i, s:i, s:i}", "a", &xi[0], "b", &xi[1], "c", &xi[2]);
	U("42", "z");
	U("null", "[i]");
	U("[]", "[}");
	U("{}", "{]");
	U("[]", "[");
	U("{}", "{");
	U("[42]", "[i]a", &xi[0]);
	U("42", "ia", &xi[0]);
	U("[]", NULL);
	U("\"foo\"", "s", NULL);
	U("42", "s", NULL);
	U("42", "n");
	U("42", "b", NULL);
	U("42", "f", NULL);
	U("42", "[i]", NULL);
	U("42", "{si}", "foo", NULL);
	U("\"foo\"", "n");
	U("\"foo\"", "b", NULL);
	U("\"foo\"", "i", NULL);
	U("\"foo\"", "I", NULL);
	U("\"foo\"", "f", NULL);
	U("\"foo\"", "F", NULL);
	U("true", "s", NULL);
	U("true", "n");
	U("true", "i", NULL);
	U("true", "I", NULL);
	U("true", "f", NULL);
	U("true", "F", NULL);
	U("[42]", "[ii]", &xi[0], &xi[1]);
	U("{\"foo\":42}", "{si}", NULL, &xi[0]);
	U("{\"foo\":42}", "{si}", "baz", &xi[0]);
	U("[1,2,3]", "[iii!]", &xi[0], &xi[1], &xi[2]);
	U("[1,2,3]", "[ii!]", &xi[0], &xi[1]);
	U("[1,2,3]", "[ii]", &xi[0], &xi[1]);
	U("[1,2,3]", "[ii*]", &xi[0], &xi[1]);
	U("{\"foo\":42,\"baz\":45}", "{sisi}", "baz", &xi[0], "foo", &xi[1]);
	U("{\"foo\":42,\"baz\":45}", "{sisi*}", "baz", &xi[0], "foo", &xi[1]);
	U("{\"foo\":42,\"baz\":45}", "{sisi!}", "baz", &xi[0], "foo", &xi[1]);
	U("{\"foo\":42,\"baz\":45}", "{si}", "baz", &xi[0], "foo", &xi[1]);
	U("{\"foo\":42,\"baz\":45}", "{si*}", "baz", &xi[0], "foo", &xi[1]);
	U("{\"foo\":42,\"baz\":45}", "{si!}", "baz", &xi[0], "foo", &xi[1]);
	U("[1,{\"foo\":2,\"bar\":null},[3,4]]", "[i{sisn}[ii]]", &xi[0], "foo", &xi[1], "bar", &xi[2], &xi[3]);
	U("[1,2,3]", "[ii!i]", &xi[0], &xi[1], &xi[2]);
	U("[1,2,3]", "[ii*i]", &xi[0], &xi[1], &xi[2]);
	U("{\"foo\":1,\"bar\":2}", "{si!si}", "foo", &xi[1], "bar", &xi[2]);
	U("{\"foo\":1,\"bar\":2}", "{si*si}", "foo", &xi[1], "bar", &xi[2]);
	U("{\"foo\":{\"baz\":null,\"bar\":null}}", "{s{sn!}}", "foo", "bar");
	U("[[1,2,3]]", "[[ii!]]", &xi[0], &xi[1]);
	U("{}", "{s?i}", "foo", &xi[0]);
	U("{\"foo\":1}", "{s?i}", "foo", &xi[0]);
	U("{}", "{s?[ii]s?{s{si!}}}", "foo", &xi[0], &xi[1], "bar", "baz", "quux", &xi[2]);
	U("{\"foo\":[1,2]}", "{s?[ii]s?{s{si!}}}", "foo", &xi[0], &xi[1], "bar", "baz", "quux", &xi[2]);
	U("{\"bar\":{\"baz\":{\"quux\":15}}}", "{s?[ii]s?{s{si!}}}", "foo", &xi[0], &xi[1], "bar", "baz", "quux", &xi[2]);
	U("{\"foo\":{\"bar\":4}}", "{s?{s?i}}", "foo", "bar", &xi[0]);
	U("{\"foo\":{}}", "{s?{s?i}}", "foo", "bar", &xi[0]);
	U("{}", "{s?{s?i}}", "foo", "bar", &xi[0]);
	U("{\"foo\":42,\"baz\":45}", "{s?isi!}", "baz", &xi[0], "foo", &xi[1]);
	U("{\"foo\":42}", "{s?isi!}", "baz", &xi[0], "foo", &xi[1]);
	return 0;
}

#endif

#if 0


    /* Unpack the same item twice */
    j = json_pack("{s:s, s:i, s:b}", "foo", "bar", "baz", 42, "quux", 1);
    if(!json_unpack_ex(j, &error, 0, "{s:s,s:s!}", "foo", &s, "foo", &s))
        fail("json_unpack object with strict validation failed");
    {
        const char *possible_errors[] = {
            "2 object item(s) left unpacked: baz, quux",
            "2 object item(s) left unpacked: quux, baz"
        };
        check_errors(possible_errors, 2, "<validation>", 1, 10, 10);
    }
    json_decref(j);

#endif

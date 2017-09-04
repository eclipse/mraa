WRAP-JSON facility
==================

The facility wrap-json is based on the pack/unpack API on the
libray jansson. The two chapters below are copied from the
documentation of jansson library copyrighted by Petri Lehtinen
(see at end).

Building Values
---------------

This section describes functions that help to create, or *pack*, complex
JSON values, especially nested objects and arrays. Value building is
based on a *format string* that is used to tell the functions about the
expected arguments.

For example, the format string `"i"` specifies a single integer value,
while the format string `"[ssb]"` or the equivalent `"[s, s, b]"`
specifies an array value with two strings and a boolean as its items:

    /* Create the JSON integer 42 */
    wrap_json_pack(&result, "i", 42);

    /* Create the JSON array ["foo", "bar", true] */
    wrap_json_pack(&result, "[ssb]", "foo", "bar", 1);

Here's the full list of format specifiers. The type in parentheses
denotes the resulting JSON type, and the type in brackets (if any)
denotes the C type that is expected as the corresponding argument or
arguments.

`s` (string) \[const char \*\]

:   Convert a null terminated UTF-8 string to a JSON string.

`s?` (string) \[const char \*\]

:   Like `s`, but if the argument is *NULL*, output a JSON null value.

`s*` (string) \[const char \*\]

:   Like `s`, but if the argument is *NULL*, do not output any value.
    This format can only be used inside an object or an array. If used
    inside an object, the corresponding key is additionally suppressed
    when the value is omitted. See below for an example.

`s#` (string) \[const char \*, int\]

:   Convert a UTF-8 buffer of a given length to a JSON string.

`s%` (string) \[const char \*, size\_t\]

:   Like `s#` but the length argument is of type size\_t.

`+` \[const char \*\]

:   Like `s`, but concatenate to the previous string. Only valid after
    `s`, `s#`, `+` or `+#`.

`+#` \[const char \*, int\]

:   Like `s#`, but concatenate to the previous string. Only valid after
    `s`, `s#`, `+` or `+#`.

`+%` (string) \[const char \*, size\_t\]

:   Like `+#` but the length argument is of type size\_t.

`n` (null)

:   Output a JSON null value. No argument is consumed.

`b` (boolean) \[int\]

:   Convert a C int to JSON boolean value. Zero is converted to `false`
    and non-zero to `true`.

`i` (integer) \[int\]

:   Convert a C int to JSON integer.

`I` (integer) \[json\_int\_t\]

:   Convert a C json\_int\_t to JSON integer.

`f` (real) \[double\]

:   Convert a C double to JSON real.

`o` (any value) \[json\_t \*\]

:   Output any given JSON value as-is. If the value is added to an array
    or object, the reference to the value passed to `o` is stolen by the
    container.

`O` (any value) \[json\_t \*\]

:   Like `o`, but the argument's reference count is incremented. This is
    useful if you pack into an array or object and want to keep the
    reference for the JSON value consumed by `O` to yourself.

`o?`, `O?` (any value) \[json\_t \*\]

:   Like `o` and `O`, respectively, but if the argument is *NULL*,
    output a JSON null value.

`o*`, `O*` (any value) \[json\_t \*\]

:   Like `o` and `O`, respectively, but if the argument is *NULL*, do
    not output any value. This format can only be used inside an object
    or an array. If used inside an object, the corresponding key is
    additionally suppressed. See below for an example.

`[fmt]` (array)

:   Build an array with contents from the inner format string. `fmt` may
    contain objects and arrays, i.e. recursive value building is
    supported.

`{fmt}` (object)

:   Build an object with contents from the inner format string `fmt`.
    The first, third, etc. format specifier represent a key, and must be
    a string (see `s`, `s#`, `+` and `+#` above), as object keys are
    always strings. The second, fourth, etc. format specifier represent
    a value. Any value may be an object or array, i.e. recursive value
    building is supported.

Whitespace, `:` and `,` are ignored.

More examples:

    /* Build an empty JSON object */
    wrap_json_pack(&result, "{}");

    /* Build the JSON object {"foo": 42, "bar": 7} */
    wrap_json_pack(&result, "{sisi}", "foo", 42, "bar", 7);

    /* Like above, ':', ',' and whitespace are ignored */
    wrap_json_pack(&result, "{s:i, s:i}", "foo", 42, "bar", 7);

    /* Build the JSON array [[1, 2], {"cool": true}] */
    wrap_json_pack(&result, "[[i,i],{s:b}]", 1, 2, "cool", 1);

    /* Build a string from a non-null terminated buffer */
    char buffer[4] = {'t', 'e', 's', 't'};
    wrap_json_pack(&result, "s#", buffer, 4);

    /* Concatenate strings together to build the JSON string "foobarbaz" */
    wrap_json_pack(&result, "s++", "foo", "bar", "baz");

    /* Create an empty object or array when optional members are missing */
    wrap_json_pack(&result, "{s:s*,s:o*,s:O*}", "foo", NULL, "bar", NULL, "baz", NULL);
    wrap_json_pack(&result, "[s*,o*,O*]", NULL, NULL, NULL);

Parsing and Validating Values
-----------------------------

This section describes functions that help to validate complex values
and extract, or *unpack*, data from them. Like building values
&lt;apiref-pack&gt;, this is also based on format strings.

While a JSON value is unpacked, the type specified in the format string
is checked to match that of the JSON value. This is the validation part
of the process. In addition to this, the unpacking functions can also
check that all items of arrays and objects are unpacked. This check be
enabled with the format specifier `!` or by using the flag
`JSON_STRICT`. See below for details.

Here's the full list of format specifiers. The type in parentheses
denotes the JSON type, and the type in brackets (if any) denotes the C
type whose address should be passed.

`s` (string) \[const char \*\]

:   Convert a JSON string to a pointer to a null terminated UTF-8
    string. The resulting string is extracted by using
    json\_string\_value() internally, so it exists as long as there are
    still references to the corresponding JSON string.

`s%` (string) \[const char \*, size\_t \*\]

:   Convert a JSON string to a pointer to a null terminated UTF-8 string
    and its length.

`n` (null)

:   Expect a JSON null value. Nothing is extracted.

`b` (boolean) \[int\]

:   Convert a JSON boolean value to a C int, so that `true` is converted
    to 1 and `false` to 0.

`i` (integer) \[int\]

:   Convert a JSON integer to C int.

`I` (integer) \[json\_int\_t\]

:   Convert a JSON integer to C json\_int\_t.

`f` (real) \[double\]

:   Convert a JSON real to C double.

`F` (integer or real) \[double\]

:   Convert a JSON number (integer or real) to C double.

`o` (any value) \[json\_t \*\]

:   Store a JSON value with no conversion to a json\_t pointer.

`O` (any value) \[json\_t \*\]

:   Like `O`, but the JSON value's reference count is incremented.

`[fmt]` (array)

:   Convert each item in the JSON array according to the inner format
    string. `fmt` may contain objects and arrays, i.e. recursive value
    extraction is supported.

`{fmt}` (object)

:   Convert each item in the JSON object according to the inner format
    string `fmt`. The first, third, etc. format specifier represent a
    key, and must be `s`. The corresponding argument to unpack functions
    is read as the object key. The second fourth, etc. format specifier
    represent a value and is written to the address given as the
    corresponding argument. **Note** that every other argument is read
    from and every other is written to.

    `fmt` may contain objects and arrays as values, i.e. recursive value
    extraction is supported.

`!`

:   This special format specifier is used to enable the check that all
    object and array items are accessed, on a per-value basis. It must
    appear inside an array or object as the last format specifier before
    the closing bracket or brace.

`*`

:   This special format specifier is the opposite of `!`. This is the default.
    It must appear inside an array or object as the last format specifier
    before the closing bracket or brace.

Whitespace, `:` and `,` are ignored.

Examples:

    /* root is the JSON integer 42 */
    int myint;
    wrap_json_unpack(root, "i", &myint);
    assert(myint == 42);

    /* root is the JSON object {"foo": "bar", "quux": true} */
    const char *str;
    int boolean;
    wrap_json_unpack(root, "{s:s, s:b}", "foo", &str, "quux", &boolean);
    assert(strcmp(str, "bar") == 0 && boolean == 1);

    /* root is the JSON array [[1, 2], {"baz": null} */
    wrap_json_check(root, "[[i,i], {s:n}]", "baz");
    /* returns 0 for validation success, nothing is extracted */

    /* root is the JSON array [1, 2, 3, 4, 5] */
    int myint1, myint2;
    wrap_json_unpack(root, "[ii!]", &myint1, &myint2);
    /* returns -1 for failed validation */

    /* root is an empty JSON object */
    int myint = 0, myint2 = 0, myint3 = 0;
    wrap_json_unpack(root, "{s?i, s?[ii]}",
                "foo", &myint1,
                "bar", &myint2, &myint3);
    /* myint1, myint2 or myint3 is no touched as "foo" and "bar" don't exist */


Copyright
---------

Copyright (c) 2009-2016 Petri Lehtinen <petri@digip.org>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.


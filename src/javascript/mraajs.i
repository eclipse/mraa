%module (docstring="Javascript interface to libmraa") mraa

%feature("autodoc", "3");

%array_class(uint8_t, uint8Array);

%inline %{
  #include <node_buffer.h>
%}

// i2c write()
%typemap(in) (const uint8_t *data, int length) {
  $1 = (uint8_t*) node::Buffer::Data($input);
  $2 = node::Buffer::Length($input);
}

%newobject I2c::read(uint8_t *data, int length);

%typemap(in) (uint8_t *data, int length) {
   int x;
   int ecode = SWIG_AsVal_int($input, &x);
   if (!SWIG_IsOK(ecode)) {
     SWIG_exception_fail(SWIG_ArgError(ecode), "Expected an int");
   }
   $2 = x;
   if ($2 < 0) {
       SWIG_exception_fail(SWIG_ERROR, "Positive integer expected");
       SWIGV8_RETURN(v8::Undefined());
   }
   $1 = (uint8_t*) malloc($2 * sizeof(uint8_t));
}

%typemap(argout) (uint8_t *data, int length) {
   if (result < 0) {      /* Check for I/O error */
       free($1);
       SWIG_exception_fail(SWIG_ERROR, "I2c write failed");
       SWIGV8_RETURN(v8::Undefined());
   }
   // Append output value $1 to $result
   // return the v8::handle of the node_buffer
   $result = node::Buffer::New((char*) $1, result)->handle_;
   free($1);
}

%include ../mraa.i

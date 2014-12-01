{
  'targets': [
    {
      'target_name': 'mraa',
      'sources': [
@mraa_LIB_SRCS_GYP@
'src/version.c',
'src/mraajsJAVASCRIPT_wrap.cxx' ],
      'include_dirs': [
@mraa_LIB_INCLUDE_DIRS_GYP@
                      ],
      'cflags_cc!': [ '-fno-rtti', '-fno-exceptions' ],
      'cflags!': [ '-fno-exceptions' ],
      'defines' : [ 'SWIG',
                    'SWIGNODE',
		    'BUILDING_NODE_EXTENSION=1' ],
    }
  ]
}

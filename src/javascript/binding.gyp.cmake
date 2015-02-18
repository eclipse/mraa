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
                    'SWIGJAVASCRIPT',
		    'BUILDING_NODE_EXTENSION=1' ],
      'conditions' : [
          [ 'target_arch=="x64"',
            { 'defines' : [ 'X86PLAT=ON' ], },
          ],
          [ 'target_arch=="ia32"',
            { 'defines' : [ 'X86PLAT=ON' ], },
          ],
          [ 'target_arch=="arm"',
            { 'defines' : [ 'ARMPLAT=ON' ], },
          ],
      ],
    }
  ]
}

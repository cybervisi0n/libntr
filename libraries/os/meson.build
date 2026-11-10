subdir('src')

libos_includes = []
if(sdk_version_major == 5)
    libos_includes += include_directories('include')
endif

libos = library('os',
    c_pch: nitro_pch,
    include_directories: [libos_includes, nitrosdk_includes],
    pic: build_target == 'nx',
    sources: [libos_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)

if (build_target == 'arm')
libos_arm7 = library('os_arm7',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libos_arm7_srcs, fx_const_h],
    dependencies: [nitrosdk_arm7_args_dep]
)

nitrosdk_arm7_libs += libos_arm7
endif

nitrosdk_libs += libos

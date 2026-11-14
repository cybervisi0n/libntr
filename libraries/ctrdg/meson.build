subdir('src')

libctrdg_includes = include_directories('include')

libctrdg = library('ctrdg',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libctrdg_includes],
    pic: (build_target == 'nx' or build_target == 'ounce'),
    sources: [libctrdg_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)

if (build_target == 'arm')
libctrdg_arm7 = library('ctrdg_arm7',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libctrdg_includes],
    pic: false,
    sources: [libctrdg_arm7_srcs, fx_const_h],
    dependencies: [nitrosdk_arm7_args_dep]
)

nitrosdk_arm7_libs += libctrdg_arm7
endif

nitrosdk_libs += libctrdg

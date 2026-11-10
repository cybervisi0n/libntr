subdir('src')

libcard_includes = include_directories('include')

libcard = library('card',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libcard_includes],
    pic: build_target == 'nx',
    sources: [libcard_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)

if (build_target == 'arm')
libcard_arm7 = library('card_arm7',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libcard_includes],
    pic: false,
    sources: [libcard_arm7_srcs, fx_const_h],
    dependencies: [nitrosdk_arm7_args_dep]
)

nitrosdk_arm7_libs += libcard_arm7
endif

nitrosdk_libs += libcard
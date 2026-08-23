subdir('src')

if (build_target == 'arm')
libinit = library('init',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libinit_srcs, fx_const_h]
)

libinit_arm7 = library('init_arm7',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libinit_srcs, fx_const_h],
    dependencies: [nitrosdk_arm7_args_dep]
)
else
libinit = library('init',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: build_target == 'nx',
    sources: [libinit_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)

libinit_arm7 = library('init_arm7',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libinit_srcs, fx_const_h],
    dependencies: [nitrosdk_arm7_args_dep]
)
endif

crt0_obj = libinit.extract_all_objects(recursive: true)
crt0_obj_arm7 = libinit_arm7.extract_all_objects(recursive: true)


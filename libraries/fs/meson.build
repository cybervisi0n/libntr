subdir('src')

libfs_includes = include_directories('include')

libfs = library('fs',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libfs_includes],
    pic: (build_target == 'nx' or build_target == 'ounce'),
    sources: [libfs_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)

if (build_target == 'arm')
libfs_arm7 = library('fs_arm7',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libfs_includes],
    pic: false,
    sources: [libfs_srcs, fx_const_h],
    dependencies: [nitrosdk_arm7_args_dep]
)

nitrosdk_arm7_libs += libfs_arm7
endif

nitrosdk_libs += libfs

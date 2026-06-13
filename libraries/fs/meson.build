subdir('src')

libfs_includes = include_directories('include')

if (build_target == 'arm')
libfs = library('fs',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libfs_includes],
    pic: false,
    sources: [libfs_srcs, fx_const_h]
)
else
libfs = library('fs',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libfs_includes],
    pic: false,
    sources: [libfs_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libfs

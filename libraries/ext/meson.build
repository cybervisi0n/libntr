subdir('src')

if (build_target == 'arm')
libext = library('ext',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libext_srcs, fx_const_h]
)
else
libext = library('ext',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libext_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libext

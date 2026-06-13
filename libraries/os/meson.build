subdir('src')

if (build_target == 'arm')
libos = library('os',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libos_srcs, fx_const_h]
)
else
libos = library('os',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libos_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libos

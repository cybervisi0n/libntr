subdir('src')

if (build_target == 'arm')
libfx = library('fx',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libfx_srcs, fx_const_h]
)
else
libfx = library('fx',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libfx_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libfx

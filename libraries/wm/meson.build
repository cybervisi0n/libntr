subdir('src')

libwm_includes = include_directories('include')

if (build_target == 'arm')
libwm = library('wm',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libwm_includes],
    pic: false,
    sources: [libwm_srcs, fx_const_h]
)
else
libwm = library('wm',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libwm_includes],
    pic: false,
    sources: [libwm_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libwm

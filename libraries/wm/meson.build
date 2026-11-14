subdir('src')

libwm_includes = include_directories('include')

libwm = library('wm',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libwm_includes],
    pic: (build_target == 'nx' or build_target == 'ounce'),
    sources: [libwm_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrosdk_libs += libwm

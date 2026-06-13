subdir('src')

if (build_target == 'arm')
libmath = library('math',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libmath_srcs, fx_const_h]
)
else
libmath = library('math',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libmath_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libmath

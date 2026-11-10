subdir('src')


libmath = library('math',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: build_target == 'nx',
    sources: [libmath_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)

if (build_target == 'arm')
libmath_arm7 = library('math_arm7',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libmath_arm7_srcs, fx_const_h],
    dependencies: [nitrosdk_arm7_args_dep]
)

nitrosdk_arm7_libs += libmath_arm7
endif

nitrosdk_libs += libmath

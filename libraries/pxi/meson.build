subdir('src')

if (build_target == 'arm')
libpxi = library('pxi',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libpxi_srcs, fx_const_h]
)

libpxi_arm7 = library('pxi_arm7',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libpxi_srcs, fx_const_h],
    dependencies: [nitrosdk_arm7_args_dep]
)
else
libpxi = library('pxi',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: build_target == 'nx',
    sources: [libpxi_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)
endif

nitrosdk_libs += libpxi
if(build_target == 'arm')
    nitrosdk_arm7_libs += libpxi_arm7
endif

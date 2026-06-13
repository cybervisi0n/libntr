subdir('src')

if (build_target == 'arm')
libpxi = library('pxi',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libpxi_srcs, fx_const_h]
)
else
libpxi = library('pxi',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libpxi_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libpxi

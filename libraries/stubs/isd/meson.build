subdir('src')

if (build_target == 'arm')
libstubsisd = library('stubsisd',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libstubsisd_srcs, fx_const_h]
)
else
libstubsisd = library('stubsisd',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libstubsisd_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libstubsisd

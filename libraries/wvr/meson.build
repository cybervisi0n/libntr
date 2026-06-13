subdir('src')

if (build_target == 'arm')
libwvr = library('wvr',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libwvr_srcs, fx_const_h]
)
else
libwvr = library('wvr',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libwvr_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libwvr

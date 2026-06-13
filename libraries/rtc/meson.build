subdir('src')

if (build_target == 'arm')
librtc = library('rtc',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [librtc_srcs, fx_const_h]
)
else
librtc = library('rtc',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [librtc_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += librtc

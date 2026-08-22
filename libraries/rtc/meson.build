subdir('src')

if (build_target == 'arm')
librtc = library('rtc',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [librtc_srcs, fx_const_h]
)

librtc_arm7 = library('rtc_arm7',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [librtc_arm7_srcs, fx_const_h],
    dependencies: [nitrosdk_arm7_args_dep]
)
else
librtc = library('rtc',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: build_target == 'nx',
    sources: [librtc_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)
endif

nitrosdk_libs += librtc
if(build_target == 'arm')
    nitrosdk_arm7_libs += librtc_arm7
endif

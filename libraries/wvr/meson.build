subdir('src')

if (build_target == 'arm')
libwvr = library('wvr',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libwvr_srcs, fx_const_h]
)

libwvr_arm7 = library('wvr_arm7',
    c_pch: nitro_pch,
    include_directories: [libwm_arm7_includes, nitrosdk_includes],
    pic: false,
    sources: [libwvr_arm7_srcs, fx_const_h],
    dependencies: [nitrosdk_arm7_args_dep]
)
else
libwvr = library('wvr',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: build_target == 'nx',
    sources: [libwvr_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)
endif

nitrosdk_libs += libwvr
if(build_target == 'arm')
    nitrosdk_arm7_libs += libwvr_arm7
endif

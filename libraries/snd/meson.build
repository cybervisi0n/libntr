subdir('src')

if (build_target == 'arm')
libsnd = library('snd',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libsnd_srcs, fx_const_h]
)

libsnd_arm7 = library('snd_arm7',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libsnd_arm7_srcs, fx_const_h],
    dependencies: [nitrosdk_arm7_args_dep]
)
else
libsnd = library('snd',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: build_target == 'nx',
    sources: [libsnd_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)
endif

nitrosdk_libs += libsnd
if(build_target == 'arm')
    nitrosdk_arm7_libs += libsnd_arm7
endif

subdir('src')

if (build_target == 'arm')
libsnd = library('snd',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libsnd_srcs, fx_const_h]
)
else
libsnd = library('snd',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libsnd_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libsnd

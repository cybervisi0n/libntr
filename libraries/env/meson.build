subdir('src')

if (build_target == 'arm')
libenv = library('env',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libenv_srcs, fx_const_h]
)
else
libenv = library('env',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libenv_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libenv

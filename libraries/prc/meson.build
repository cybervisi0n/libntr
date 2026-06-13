subdir('src')

if (build_target == 'arm')
libprc = library('prc',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libprc_srcs, fx_const_h]
)
else
libprc = library('prc',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libprc_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libprc

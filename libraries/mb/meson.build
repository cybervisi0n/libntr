subdir('src')

libmb_includes = include_directories('include')

if (build_target == 'arm')
libmb = library('mb',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libmb_includes],
    pic: false,
    sources: [libmb_srcs, fx_const_h]
)
else
libmb = library('mb',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libmb_includes],
    pic: false,
    sources: [libmb_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libmb

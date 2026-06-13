subdir('src')

libcard_includes = include_directories('include')

if (build_target == 'arm')
libcard = library('card',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libcard_includes],
    pic: false,
    sources: [libcard_srcs, fx_const_h]
)
else
libcard = library('card',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libcard_includes],
    pic: false,
    sources: [libcard_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libcard

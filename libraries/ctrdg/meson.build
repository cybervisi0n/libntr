subdir('src')

libctrdg_includes = include_directories('include')

if (build_target == 'arm')
libctrdg = library('ctrdg',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libctrdg_includes],
    pic: false,
    sources: [libctrdg_srcs, fx_const_h]
)
else
libctrdg = library('ctrdg',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libctrdg_includes],
    pic: false,
    sources: [libctrdg_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libctrdg

subdir('src')

libel_includes = include_directories('include')

libel = library('el',
    c_pch: nitro_pch,
    include_directories: [libtwl_includes, libel_includes],
    pic: build_target == 'nx',
    sources: [libel_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)

libtwl_libs += libel

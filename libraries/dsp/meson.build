subdir('src')

libdsp_includes = include_directories('include')

libdsp = library('dsp',
    c_pch: nitro_pch,
    include_directories: [libtwl_includes, libdsp_includes],
    pic: build_target == 'nx',
    sources: [libdsp_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)

libtwl_libs += libdsp

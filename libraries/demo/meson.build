subdir('src')

libdemo = library('demo',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: build_target == 'nx',
    sources: [libdemo_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrosdk_libs += libdemo
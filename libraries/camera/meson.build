subdir('src')

libcamera_includes = include_directories('include')

libcamera = library('camera',
    c_pch: nitro_pch,
    include_directories: [libtwl_includes, libcamera_includes],
    pic: build_target == 'nx',
    sources: [libcamera_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)

libtwl_libs += libcamera

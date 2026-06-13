subdir('src')

libsim7_includes = include_directories('include')

libsim7 = library('sim7',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libsim7_includes],
    pic: false,
    sources: [libsim7_srcs, fx_const_h],
    native: true
)

nitrosdk_libs += libsim7

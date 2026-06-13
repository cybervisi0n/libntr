subdir('src')

if (build_target == 'arm')
libutest = library('utest',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libutest_srcs, fx_const_h]
)
else
libutest = library('utest',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libutest_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libutest

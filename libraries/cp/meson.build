subdir('src')

if (build_target == 'arm')
libcp = library('cp',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libcp_srcs, fx_const_h]
)
else
libcp = library('cp',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libcp_srcs, fx_const_h],
    native: true
)
endif

nitrosdk_libs += libcp
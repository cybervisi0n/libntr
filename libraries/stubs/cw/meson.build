subdir('src')

if (build_target == 'arm')
libstubscw = library('stubscw',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: false,
    sources: [libstubscw_srcs, fx_const_h]
)
else
libstubscw = library('stubscw',
    c_pch: nitro_pch,
    include_directories: nitrosdk_includes,
    pic: (build_target == 'nx' or build_target == 'ounce'),
    sources: [libstubscw_srcs, fx_const_h],
    native: (build_target == 'linux' or build_target == 'win64')
)
endif

nitrosdk_libs += libstubscw

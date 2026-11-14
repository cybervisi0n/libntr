if (build_target == 'arm')
    nitrosdk_prebuilt += fs.copyfile('prebuilt' / 'libdgt.a')
else
    subdir('src')
    libdgt = library('dgt',
        c_pch: nitro_pch,
        include_directories: nitrosdk_includes,
        pic: (build_target == 'nx' or build_target == 'ounce'),
        sources: [libdgt_srcs],
        native: (build_target == 'linux' or build_target == 'win64')
    )

    nitrosdk_libs += libdgt
endif

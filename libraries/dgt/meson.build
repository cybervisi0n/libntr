if (build_target == 'arm')
    nitrosdk_prebuilt += fs.copyfile('prebuilt' / 'libdgt.a')
else
    subdir('src')
    libdgt = library('dgt',
        c_pch: nitro_pch,
        include_directories: nitrosdk_includes,
        pic: false,
        sources: [libdgt_srcs],
        native: true
    )

    nitrosdk_libs += libdgt
endif

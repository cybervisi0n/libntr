libfs_srcs = files(
    'fs_archive.c',
    'fs_file.c',
    'fs_overlay.c'
)

if(sdk_version_major == 4)
    libfs_srcs += files(
        'fs_command.c',
        'fs_command_default.c',
        'fs_mem.c',
        'fs_rom.c'
    )
endif

if(sdk_version_major == 5)
    libfs_srcs += files(
        'fs_api.c',
        'fs_archive_fatfs.c',
        'fs_hook.c',
        'fs_proc_default.c',
        'fs_proc_mem.c',
        'fs_proc_rom.c'
    )
endif

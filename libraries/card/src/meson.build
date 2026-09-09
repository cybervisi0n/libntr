libcard_srcs = files(
    'card_common.c',
    'card_spi.c',
    'card_backup.c',
    'card_rom.c',
    'card_pullOut.c'
)

if(sdk_version_major == 4)
    libcard_srcs += files(
        'card_request.c'
    )
endif

if(sdk_version_major == 5)
    libcard_srcs += files(
        'card_api.c',
        'card_hash.c',
        'card_hook.c',
        'card_task.c',
        'card_utility.c'
    )
endif

libcard_arm7_srcs = files(
    'card_common.c',
    'card_spi.c',
    'card_backup.c',
    'card_rom.c',
    'card_request_arm7.c',
    'card_pullOut_arm7.c',
)

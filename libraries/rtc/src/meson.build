librtc_srcs = files(
    'external.c',
    'internal.c',
    'convert.c'
)

librtc_arm7_srcs = files(
    'control.c',
    'instruction.c',
    'gpio.c',
)

if(sdk_version_major == 5)
    librtc_srcs += files(
        'swclock.c'
    )
endif
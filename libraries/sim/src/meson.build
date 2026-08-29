libsim_srcs = files(
    'blip-buf/blip_buf.c',
    'config/ini.c',
    'config/sim_config.c',
    'gui/gui.c',
    'gui/gui_config.c',
    'gui/gui_g2.c',
    'gui/gui_net.c',
    'gui/gui_pad.c',
    'g3_draw.cpp',
    'g3_handler.cpp',
    'queue.c',
    'sim_assert.c',
    'sim_audio.c',
    'sim_crc32.c',
    'sim_debug.cpp',
    'sim_g2.cpp',
    'sim_main.cpp',
    'sim_net.cpp',
    'sim_screenquads.c',
    'sim_shaders.cpp'
)

if(build_target == 'linux' or build_target == 'win64')
libsim_srcs += files('glad.c')
endif

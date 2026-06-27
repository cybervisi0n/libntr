libsim_srcs = files(
    'blip-buf/blip_buf.c',
    'config/ini.c',
    'config/sim_config.c',
    'gui/gui.c',
    'gui/gui_config.c',
    'gui/gui_g2.c',
    'gui/gui_net.c',
    'gui/gui_pad.c',
    'g3_draw.c',
    'g3_handler.c',
    'g3_shader.c',
    'queue.c',
    'sim_assert.c',
    'sim_audio.c',
    'sim_debug.c',
    'sim_g2.c',
    'sim_main.c',
    'sim_net.c',
    'sim_screenquads.c'
)

if(build_target == 'linux' or build_target == 'win64')
libsim_srcs += files('glad.c')
endif

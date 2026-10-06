libsim_srcs = files(
    'blip-buf/blip_buf.c',
    'config/ini.c',
    'config/sim_config.c',
    'fireflower/nds-extract.cpp',
    'imgui/imgui.cpp',
    'imgui/imgui_demo.cpp',
    'imgui/imgui_draw.cpp',
    'imgui/imgui_impl_opengl3.cpp',
    'imgui/imgui_impl_sdl2.cpp',
    'imgui/imgui_tables.cpp',
    'imgui/imgui_widgets.cpp',
    'ImGuiFileDialog/ImGuiFileDialog.cpp',
    'g3_draw.cpp',
    'g3_handler.cpp',
    'queue.c',
    'sim_assert.c',
    'sim_audio.cpp',
    'sim_crc32.c',
    'sim_debug.cpp',
    'sim_g2.cpp',
    'sim_g3_CommandProcessor.cpp',
    'sim_g3_Texture.cpp',
    'sim_g3_TextureCache.cpp',
    'sim_gui.cpp',
    'sim_gui_config.cpp',
    'sim_gui_net.cpp',
    'sim_gui_pad.cpp',
    'sim_main.cpp',
    'sim_net.cpp',
    'sim_RomExtractor.cpp',
    'sim_screenquads.c',
    'sim_shaders.cpp',
    'sim_vars.c'
)

if(build_target == 'linux' or build_target == 'win64')
libsim_srcs += files('glad.c')
endif

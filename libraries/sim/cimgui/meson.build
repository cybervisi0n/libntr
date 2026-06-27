libcimgui_cpp_args = [
    '-DIMGUI_IMPL_OPENGL_LOADER_GLAD',
    '-Dcimgui_EXPORTS'
]

if(build_target == 'nx')
libcimgui_cpp_args += ['-I/opt/devkitpro/portlibs/switch/include', '-I/opt/devkitpro/libnx/include', '-DSDK_BUILD_NX']
endif

cimgui_includes = include_directories('imgui')

cimgui_srcs = files(
    'cimgui.cpp',
    'cimgui_impl.cpp',
    'imgui/imgui.cpp',
    'imgui/imgui_draw.cpp',
    'imgui/imgui_demo.cpp',
    'imgui/imgui_tables.cpp',
    'imgui/imgui_widgets.cpp',
    'imgui/backends/imgui_impl_sdl2.cpp',
    'imgui/backends/imgui_impl_opengl3.cpp'
)

libcimgui = library('cimgui',
    sources: cimgui_srcs,
    cpp_args: libcimgui_cpp_args,
    pic: build_target == 'nx',
    include_directories: [nitrosdk_includes, cimgui_includes],
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrosdk_libs += libcimgui

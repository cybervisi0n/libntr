subdir('src')

libsim_includes = include_directories('include')

sdl2_lib = library('SDL2')
enet_lib = library('enet')

if(build_target == 'linux' or build_target == 'win64')
sdl2_lib = meson.get_compiler('c').find_library('SDL2')
elif(build_target == 'nx')
sdl2_lib = meson.get_compiler('c').find_library('SDL2', dirs: '/opt/devkitpro/portlibs/switch/lib/')
elif(build_target == 'prospero')
sdl2_lib = meson.get_compiler('c').find_library('SDL2', dirs: prospero_lib_dirs)
endif
sdl2_dep = declare_dependency(dependencies: [sdl2_lib])

if(build_target == 'linux' or build_target == 'win64')
enet_lib = meson.get_compiler('c').find_library('enet')
enet_dep = declare_dependency(dependencies: [enet_lib])
elif(build_target == 'nx')
enet_lib = meson.get_compiler('c').find_library('enet', dirs: '/opt/devkitpro/portlibs/switch/lib/')
enet_dep = declare_dependency(dependencies: [enet_lib])
endif


libsim_dependencies = [sdl2_dep]
if(build_target == 'linux' or build_target == 'win64' or build_target == 'nx')
    libsim_dependencies += enet_dep
endif

# PS5 external libraries
if(build_target == 'prospero')
    prospero_libs = declare_dependency(
        dependencies: [
            # SCE Core libraries
            meson.get_compiler('cpp').find_library('kernel', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('SceAudioOut', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('SceImeDialog', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('SceKeyboard', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('ScePad', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('SceSystemService', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('SceUserService', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('SceVideoOut', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('SceAgc', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('SceAgcDriver', dirs: prospero_lib_dirs),

            # OpenGL libraries
            meson.get_compiler('cpp').find_library('blake3', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('compiler', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('gallium', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('glapi_bridge', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('glapi', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('glcpp', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('glsl_util', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('glsl', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('mesa_util_c11', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('mesa_util_simd', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('mesa_util_clflush', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('mesa_util_clflushopt', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('mesa_util', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('mesa', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('mesa_sse41', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('nir', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('ps5_opengl_core33', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('psbc.ps5', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('softpipe', dirs: prospero_lib_dirs),
            meson.get_compiler('cpp').find_library('vtn', dirs: prospero_lib_dirs),
        ]
    )
    libsim_dependencies += prospero_libs
endif


libsim_args = [ ]

if(build_target == 'win64')
ws2_32_lib = meson.get_compiler('c').find_library('ws2_32')

ws2_32_dep = declare_dependency(dependencies: [ws2_32_lib])
libsim_dependencies += ws2_32_dep
endif

# CARD Game code/Maker code
if(get_option('card_game_code') != '')
    card_game_code_define = '-DLIBNTR_CARD_GAME_CODE="' + get_option('card_game_code') + '"'
    libsim_args += card_game_code_define
endif
if(get_option('card_maker_code') != '')
    card_game_code_define = '-DLIBNTR_CARD_MAKER_CODE="' + get_option('card_maker_code') + '"'
    libsim_args += card_game_code_define
endif

if(get_option('tracy_enable') == true)
    if(build_target =='linux' or build_target == 'win64')
        tracy_native = dependency(
            'tracy_native', 
            default_options: {
                'no_sampling': true,
                'only_ipv4': true,
                'delayed_init': true,
                'manual_lifetime': true,
                'no_callstack': true,
            },
            static : true)
        libsim_dependencies += tracy_native
    else
        tracy = dependency(
            'tracy', 
            default_options: {
                'no_sampling': true,
                'only_ipv4': true,
                'delayed_init': true,
                'manual_lifetime': true,
                'no_callstack': true,
            },
            static : true)
        libsim_dependencies += tracy
    endif
    libsim_includes += subproject('tracy').get_variable('tracy_public_include_dirs')
    libsim_args += [
        '-DSDK_TRACY_ENABLE'
    ]
endif


if(build_target == 'nx')
glad_lib = meson.get_compiler('c').find_library('glad', dirs: '/opt/devkitpro/portlibs/switch/lib/')
glad_dep = declare_dependency(dependencies: [glad_lib])
libsim_dependencies += glad_dep

libegl_lib = meson.get_compiler('c').find_library('EGL', dirs: '/opt/devkitpro/portlibs/switch/lib/')
libegl_dep = declare_dependency(dependencies: [libegl_lib])
libsim_dependencies += libegl_dep

libglapi_lib = meson.get_compiler('c').find_library('glapi', dirs: '/opt/devkitpro/portlibs/switch/lib/')
libglapi_dep = declare_dependency(dependencies: [libglapi_lib])
libsim_dependencies += libglapi_dep

libdrm_nouveau_lib = meson.get_compiler('c').find_library('drm_nouveau', dirs: '/opt/devkitpro/portlibs/switch/lib/')
libdrm_nouveau_dep = declare_dependency(dependencies: [libdrm_nouveau_lib])
libsim_dependencies += libdrm_nouveau_dep

libnx_lib = meson.get_compiler('c').find_library('nx', dirs: '/opt/devkitpro/libnx/lib/')
libnx_dep = declare_dependency(dependencies: [libnx_lib])
libsim_dependencies += libnx_dep
endif

libsim = library('sim',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libsim_includes],
    pic: build_target == 'nx',
    c_args: libsim_args,
    cpp_args: [libsim_args, '-std=c++20'],
    sources: [libsim_srcs, fx_const_h, git_info_h],
    dependencies: libsim_dependencies,
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrosdk_libs += libsim


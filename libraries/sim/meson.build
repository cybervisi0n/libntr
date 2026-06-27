subdir('cimgui')
subdir('src')

libsim_includes = include_directories('include')

sdl2_lib = library('SDL2')
enet_lib = library('enet')

if(build_target == 'linux' or build_target == 'win64')
sdl2_lib = meson.get_compiler('c').find_library('SDL2')
elif(build_target == 'nx')
sdl2_lib = meson.get_compiler('c').find_library('SDL2', dirs: '/opt/devkitpro/portlibs/switch/lib/')
endif
sdl2_dep = declare_dependency(dependencies: [sdl2_lib])

if(build_target == 'linux' or build_target == 'win64')
enet_lib = meson.get_compiler('c').find_library('enet')
elif(build_target == 'nx')
enet_lib = meson.get_compiler('c').find_library('enet', dirs: '/opt/devkitpro/portlibs/switch/lib/')
endif
enet_dep = declare_dependency(dependencies: [enet_lib])

libsim_dependencies = [sdl2_dep, enet_dep]

if(build_target =='linux' or build_target == 'win64')
tracy_native = dependency('tracy_native', static : true)
libsim_dependencies += tracy_native
else
tracy = dependency('tracy', static : true)
libsim_dependencies += tracy
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
    include_directories: [nitrosdk_includes, libsim_includes, subproject('tracy').get_variable('tracy_public_include_dirs')],
    pic: build_target == 'nx',
    sources: [libsim_srcs, fx_const_h],
    dependencies: libsim_dependencies,
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrosdk_libs += libsim


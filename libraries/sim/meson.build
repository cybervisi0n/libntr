subdir('src')

libsim_includes = include_directories('include')

sdl2_lib = meson.get_compiler('c').find_library('SDL2')
sdl2_dep = declare_dependency(dependencies: [sdl2_lib])

enet_lib = meson.get_compiler('c').find_library('enet')
enet_dep = declare_dependency(dependencies: [enet_lib])

libsim = library('sim',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libsim_includes],
    pic: false,
    sources: [libsim_srcs, fx_const_h],
    dependencies: [sdl2_dep, enet_dep],
    native: true
)

nitrosdk_libs += libsim

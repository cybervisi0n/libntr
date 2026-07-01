subdir('src')

libsim7_includes = include_directories('include')

libsim7_dependencies = []

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
libsim7_dependencies += tracy_native
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
libsim7_dependencies += tracy
endif

libsim7 = library('sim7',
    c_pch: nitro_pch,
    include_directories: [nitrosdk_includes, libsim7_includes],
    pic: build_target == 'nx',
    sources: [libsim7_srcs, fx_const_h],
    dependencies: libsim7_dependencies,
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrosdk_libs += libsim7

project('libntr', 'c', 'cpp', 'nasm',
    default_options: {
        'default_library': 'static',
        'prefer_static':    false
    },
    meson_version: '>=1.3.0',
    version: '4.2.30001'
)

fs = import('fs')

buildtype = get_option('buildtype')
cc = meson.get_compiler('c')
build_target = get_option('build_target')

############################################################
###                    COMPILER FLAGS                    ###
############################################################

if not meson.is_subproject() and buildtype != 'plain' and build_target == 'arm'
shared_args = [
    '-O4,p',
    '-proc', 'arm946e',
    '-enum', 'int',
    '-lang', 'c99',
    '-Cpp_exceptions', 'off',
    '-gccext,on',
    '-msgstyle', 'gcc',
    '-ipa', 'file',
    '-interworking',
    '-inline', 'on,noauto',
    '-char', 'signed',
    '-nosyspath',
    '-stdinc'
]
else
shared_args = []
endif

mwrap_ver = get_option('mwrap_ver')
if mwrap_ver != 'default' and build_target == 'arm'
    shared_args += ['-wrap:ver', mwrap_ver]
endif

mwrap_sdk = get_option('mwrap_sdk')
if mwrap_sdk != 'default' and build_target == 'arm'
  shared_args += ['-wrap:sdk', mwrap_sdk]
endif

if get_option('sp1p3_conditionals')
    shared_args += '-DSP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS'
endif

c_args = shared_args
cpp_args = shared_args

add_project_arguments(c_args,
    language: 'c',
    native: (build_target == 'linux' or build_target == 'win64')
)

add_project_arguments(cpp_args,
    language: 'c',
    native: (build_target == 'linux' or build_target == 'win64')
)

nitrosdk_args = [
    '-DSDK_CW_FORCE_EXPORT_SUPPORT',
    '-DSDK_TS',
    '-DSDK_4M',
    '-DSDK_FINALROM',
    '-DNNS_FINALROM'
]

if (build_target == 'arm')
    nitrosdk_args += [
        '-DSDK_ARM9',
        '-DSDK_CW',
        '-DSDK_CODE_ARM',
        '-DSDK_BUILD_ARM'
    ]
endif

if (build_target == 'win64')
    nitrosdk_args += [
        '-DSDK_PORT',
        '-DSDK_X86',
        '-DSDK_CODE_X86',
        '-DSDK_BUILD_WIN64',
        '-D_NITRO',
        '-w'
    ]
endif

if (build_target == 'linux')
    nitrosdk_args += [
        '-DSDK_PORT',
        '-DSDK_X86',
        '-DSDK_CODE_X86',
        '-DSDK_BUILD_LINUX',
        '-D_NITRO',
        '-w',
    ]
endif

if (build_target == 'nx')
    nitrosdk_args += [
        '-DSDK_PORT',
        '-DSDK_AARCH64',
        '-DSDK_BUILD_NX',
        '-D_NITRO',
        '-D__SWITCH__',
        '-w',
        '-I/opt/devkitpro/portlibs/switch/include',
        '-I/opt/devkitpro/libnx/include'
    ]
endif

nitrosdk_arm7_args = []

if(build_target == 'arm')
    nitrosdk_arm7_args = [
        '-proc', 'arm7tdmi',
        '-USDK_ARM9',
        '-DSDK_ARM7',
        '-DSP1P3_BUG_FOR_CONDITIONAL_ASM_INSTRUCTIONS',
        '-O4,s',
        '-sym', 'on',
        '-fp', 'soft',
        '-wrap:sdk', '2.0/sp1p2',
    ]
else
    nitrosdk_arm7_args = nitrosdk_args
endif


nitrosdk_arm7_args_dep = declare_dependency(
  compile_args: nitrosdk_arm7_args
)

add_project_arguments(nitrosdk_args,
    language: 'c',
    native: (build_target == 'linux' or build_target == 'win64')
)

add_project_arguments(nitrosdk_args,
    language: 'cpp',
    native: (build_target == 'linux' or build_target == 'win64')
)

############################################################
###                  GENERATED HEADERS                   ###
############################################################
subdir('gen')
nitro_pch = meson.current_source_dir() / 'include/pch/nitro_pch.h'

############################################################
###                    INCLUDE PATHS                     ###
############################################################
nitrosdk_includes = include_directories('gen', 'include')
include_nitro = include_directories('include' / 'nitro')
wl_includes = include_directories('include' / 'nitro_wl' / 'ARM7')

############################################################
###                      LIBRARIES                       ###
############################################################
subdir('libraries')

############################################################
###                      SPECFILES                       ###
############################################################
subdir('specfiles')

############################################################
###                        TOOLS                         ###
############################################################
subdir('tools')

nitrosdk_arm7_dep = declare_dependency(
    compile_args: nitrosdk_args + nitrosdk_arm7_args,
    include_directories: nitrosdk_includes,
    link_with: nitrosdk_arm7_libs,
    objects: crt0_obj_arm7,
    sources: fx_const_h
)

############################################################
###                ARM7 PREBUILT BINARIES                ###
############################################################
subdir('components')

############################################################
###                    FOR CONSUMERS                     ###
############################################################

nitrosdk_dep = declare_dependency(
    compile_args: nitrosdk_args,
    include_directories: nitrosdk_includes,
    link_with: nitrosdk_libs + nitrosdk_prebuilt,
    objects: crt0_obj,
    sources: fx_const_h
)

meson.override_dependency('NitroSDK', nitrosdk_dep)

if(get_option('standalone') == true)
    subdir('standalone')
endif

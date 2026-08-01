libntr_link_args = []

libntr_exe = executable(
    'libntr',
    sources: files('main.cpp'),
    dependencies: [ nitrosdk_dep ],
    link_args: [ libntr_link_args ],
    native: build_target != 'arm',
)

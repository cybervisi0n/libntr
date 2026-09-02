libntr_link_args = []

libntr_exe = executable(
    'libntr',
    sources: files('main.c'),
    dependencies: [ nitrosdk_dep ],
    link_args: [ libntr_link_args ],
    native: build_target != 'arm',
)

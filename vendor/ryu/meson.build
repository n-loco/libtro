# Veja file://./NOTICE.md
project(
    'ryu',
    'c',
    version: '2.0',
    meson_version: '>= 1.3.0',
    default_options: ['warning_level=0'],
    license: 'Apache 2.0',
    license_files: 'LICENSE-Apache2',
)

includes = include_directories('.')

sources = []

subdir('ryu')

lib = static_library(
    'ryu',
    sources,
    include_directories: includes,
    gnu_symbol_visibility: 'hidden',
)

ryu_dep = declare_dependency(
    include_directories: includes,
    link_with: lib,
)
meson.override_dependency('ryu', ryu_dep)

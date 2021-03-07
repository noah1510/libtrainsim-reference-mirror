project(
    'simulator', 
    'c', 
    'cpp',
    default_options : [
        'c_std=c11', 
        'cpp_std=c++17',
        'b_lto=true',
        'b_pgo=generate',
        'b_coverage=true',
        'libtrainsim:build_full=true'
    ],
    version:'0.3.0',
    meson_version: '>= 0.54.0',
)

libtrainsim_version = '0.4.0'

deps = [
    dependency('libtrainsim-full', required: true, fallback: 'libtrainsim', version : '>=' + libtrainsim_version),
]

incdir = include_directories('include')

sources = [
    'src/main.cpp',
    'src/simulator.cpp',
]

executable(
    'simulator', 
    sources,
    include_directories : incdir,
    dependencies: deps,
    install : true,
    install_dir : get_option('datadir') / 'libtrainsim',
)

install_subdir('data', install_dir : get_option('datadir') / 'libtrainsim')

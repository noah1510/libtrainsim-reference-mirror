project(
    'simulator', 
    'c', 
    'cpp',
    default_options : [
        'c_std=c11', 
        'cpp_std=c++17'
    ],
    version:'0.2.0',
    meson_version: '>= 0.54.0',
)

libtrainsim_version = '0.2.0'

deps = [
    dependency('libtrainsim-video', required: true, fallback: 'libtrainsim', version : '>=' + libtrainsim_version),
    dependency('libtrainsim-control', required: true, fallback: 'libtrainsim', version : '>=' + libtrainsim_version),
    dependency('libtrainsim-core', required: true, fallback: 'libtrainsim', version : '>=' + libtrainsim_version),
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
    dependencies: deps
)

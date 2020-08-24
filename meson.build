project(
    'simulator', 
    'c', 
    'cpp',
    default_options : [
        'c_std=c11', 
        'cpp_std=c++17'
    ],
    version:'0.1.0',
    meson_version: '>= 0.54.0',
)

deps = [
    dependency('redhand', required : true, fallback : ['redhand', 'redhand_dep']),
    dependency('libtrainsim-video', required: true, fallback: 'libtrainsim'),
    dependency('libtrainsim-control', required: true, fallback: 'libtrainsim'),
]

incdir = include_directories('include')

sources = [
    'src/main.cpp', 
    'src/simulator_world.cpp'
]

executable(
    'simulator', 
    sources,
    include_directories : incdir,
    dependencies: deps
)

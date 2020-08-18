project(
    'simulator', 
    'c', 
    'cpp',
    default_options : [
        'c_std=c11', 
        'cpp_std=c++17'
    ]
)

redhand_dep = dependency('redhand', required : true, fallback : ['redhand', 'redhand_dep'])

incdir = include_directories('include')

sources = [
    'src/main.cpp', 
    'src/simulator_world.cpp'
]

executable(
    'simulator', 
    sources,
    include_directories : incdir,
    dependencies: redhand_dep
)

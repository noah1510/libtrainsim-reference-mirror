project(
    'simulator',
    'c',
    'cpp',
    default_options : [
        'c_std=c11',
        'cpp_std=c++17',
        'b_lto=true',
        'b_pgo=generate',
        'b_coverage=false',
        'libtrainsim:build_full=true',
    ],
    version : '0.9.2',
    meson_version : '>= 0.54.0',
)

#make the best out of the cpu arch
march = get_option('march')

add_global_arguments('-march=' + march, language : 'c')
add_global_arguments('-march=' + march, language : 'cpp')

#the minimum required libtrainsim version
libtrainsim_version = '0.9.2'

deps = [
    dependency('libtrainsim-full', required: true, fallback: 'libtrainsim', version : '>=' + libtrainsim_version),
]

incdir = include_directories('include')

sources = [
    'src/main.cpp',
    'src/simulator.cpp',
]

if target_machine.system() == 'emscripten'
    executable(
        'simulator', 
        sources,
        include_directories : incdir,
        dependencies: deps,
        install : true,
        install_dir : get_option('datadir') / 'libtrainsim',
    )
else
    
    executable(
        'simulator', 
        sources,
        include_directories : incdir,
        dependencies: deps,
        install : true,
        install_dir : get_option('datadir') / 'libtrainsim',
    )
    
endif

if not get_option('no-data')
    install_subdir('data', install_dir : get_option('datadir') / 'libtrainsim')
endif

project(
    'simulator',
    'c',
    'cpp',
    default_options : [
        'c_std=c11',
        'cpp_std=c++17',
        'b_coverage=false',
        'libtrainsim:build_full=true',
        'b_lto=true',
        'b_pgo=off',
        'warning_level=3'
    ],
    version : '0.11.0',
    meson_version : '>= 0.54.0',
)

compiler = meson.get_compiler('cpp')
if compiler.get_id() != 'msvc'
    #make the best out of the cpu arch
    march = get_option('march')

    add_global_arguments('-march=' + march, language : 'c')
    add_global_arguments('-march=' + march, language : 'cpp')
endif

#the minimum required libtrainsim version
libtrainsim_version = meson.project_version()

deps = [
    dependency('libtrainsim-full', required: true, fallback: 'libtrainsim', version : '>=' + libtrainsim_version),
]

incdir = include_directories('include')

sources = [
    'src/main.cpp',
    'src/simulator.cpp',
]

#try to check if all individual shader files are valid
if not get_option('no-data')
    subdir('data/production_data/shaders')
endif

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

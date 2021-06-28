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
    version:'0.8.0',
    meson_version: '>= 0.54.0',
)

#make the best out of the x86_64/amd64 arch
if target_machine.cpu_family() == 'x86_64'
    march = get_option('march_x86_64')

    add_global_arguments('-march=' + march, language : 'c')
    add_global_arguments('-march=' + march, language : 'cpp')
endif

#make the best out of the arm arch
if target_machine.cpu_family() == 'aarch64'
    march = get_option('march_arm_64')

    add_global_arguments('-march=' + march, language : 'c')
    add_global_arguments('-march=' + march, language : 'cpp')
endif

if target_machine.cpu_family() == 'arm'
    march = get_option('march_arm_32')

    add_global_arguments('-march=' + march, language : 'c')
    add_global_arguments('-march=' + march, language : 'cpp')
endif

#make the best out of the power pc arch
if target_machine.cpu_family() == 'ppc' or target_machine.cpu_family() == 'ppc64'
    add_global_arguments('-mcpu=native', language : 'c')
    add_global_arguments('-mcpu=native', language : 'cpp')
endif


libtrainsim_version = '0.7.0'

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

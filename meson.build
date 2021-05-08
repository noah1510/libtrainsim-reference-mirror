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
        'unit-systen:build_full=true',
    ],
    version:'0.4.1',
    meson_version: '>= 0.54.0',
)

#make the best out of the x86_64/amd64 arch
if build_machine.cpu_family() == 'x86_64'
    march = get_option('march_x86_64')

    add_global_arguments('-march=' + march, language : 'c')
    add_global_arguments('-march=' + march, language : 'cpp')
endif

#make the best out of the arm arch
if build_machine.cpu_family() == 'aarch64'
    march = get_option('march_arm_64')

    add_global_arguments('-march=' + march, language : 'c')
    add_global_arguments('-march=' + march, language : 'cpp')
endif

if build_machine.cpu_family() == 'arm'
    march = get_option('march_arm_32')

    add_global_arguments('-march=' + march, language : 'c')
    add_global_arguments('-march=' + march, language : 'cpp')
endif

#make the best out of the power pc arch
if build_machine.cpu_family() == 'ppc' or build_machine.cpu_family() == 'ppc64'
    add_global_arguments('-mcpu=native', language : 'c')
    add_global_arguments('-mcpu=native', language : 'cpp')
endif


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

if not get_option('no-data')
    install_subdir('data', install_dir : get_option('datadir') / 'libtrainsim')
endif

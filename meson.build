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
    meson_version : '>= 0.56.0',
)

compiler = meson.get_compiler('cpp')
if compiler.get_id() != 'msvc'
    #make the best out of the cpu arch
    march = get_option('march')

    add_global_arguments('-march=' + march, language : 'c')
    add_global_arguments('-march=' + march, language : 'cpp')
else
    vcpkg_path = get_option('vcpkg-path')
    if vcpkg_path == ''
        error('vcpkg-path is needed to compile with msvc')
    endif

    packagePath = vcpkg_path / 'installed/x64-windows'
    sdlMainLinkArg = '-l'+packagePath / 'lib/manual-link/SDL2main'
    add_global_link_arguments(sdlMainLinkArg, language : 'c')
    add_global_link_arguments(sdlMainLinkArg, language : 'cpp')

    #mark all dll to be installed
    install_subdir(packagePath, install_dir:get_option('prefix'), strip_directory: true)
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
#if not get_option('no-data')
#    subdir('data/production_data/shaders')
#endif

install_data('application-json.png',install_dir:get_option('bindir'))

executable(
    'simulator', 
    sources,
    include_directories : incdir,
    dependencies: deps,
    win_subsystem: 'windows',
    install : true,
)


if not get_option('no-data')
    install_subdir('data', install_dir : get_option('datadir') / 'libtrainsim')
endif

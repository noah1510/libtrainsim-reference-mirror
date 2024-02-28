project(
    'simulator',
    'c',
    'cpp',
    default_options : [
        'cpp_std=c++23',
        'b_coverage=false',
        'libtrainsim:build_full=false',
        'b_lto=true',
        'b_pgo=off',
        'warning_level=3'
    ],
    version : '0.12.0',
    meson_version : '>= 1.0.0',
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

driver_debug = false
if get_option('gl_debug').auto()
    driver_debug = get_option('buildtype').contains('debug')
elif get_option('gl_debug').enabled()
    driver_debug = true
endif

if driver_debug
    if target_machine.system() == 'windows'
        add_global_arguments('-DGL_WINDOWS_DEBUG', language : 'cpp')
    elif target_machine.system() == 'linux'
        add_global_arguments('-DGL_LINUX_DEBUG', language : 'cpp')
    endif
endif

deps = [
    #dependency('libtrainsim-full', required: true, fallback: 'libtrainsim', version : '>=' + libtrainsim_version),
    dependency('libtrainsim-video', required: true, fallback: 'libtrainsim', version : '>=' + libtrainsim_version),
    dependency('libtrainsim-control', required: true, fallback: 'libtrainsim', version : '>=' + libtrainsim_version),
    dependency('libtrainsim-physics', required: true, fallback: 'libtrainsim', version : '>=' + libtrainsim_version),
    dependency('trainsim-extras-statusDisplay', required: true, fallback: 'libtrainsim', version : '>=' + libtrainsim_version),
]

incdir = include_directories('include')

sources = [
    'src/main.cpp',
    'src/mainWindow.cpp',
    'src/trackSelectionWidget.cpp',
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
    cpp_pch: 'include/pch/simulator_pch.hpp',
)


if not get_option('no-data')
    install_subdir('data', install_dir : get_option('datadir') / 'libtrainsim')
endif

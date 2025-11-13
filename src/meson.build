# SPDX-License-Identifier: LGPL-2.1-or-later

sources = files(
        'glob.c',
)

#####################################################################
# internal library

libshared_static = static_library(
        'muslpolyfill-shared',
        sources,
        include_directories : includes,
        implicit_include_directories : false,
        dependencies : [userspace],
        c_args : ['-fvisibility=default'],
        build_by_default : false)

#####################################################################
# public library

libmuslpolyfill_sym = files('libmuslpolyfill.sym')
libmuslpolyfill_sym_path = meson.current_source_dir() / 'libmuslpolyfill.sym'

libmuslpolyfill_pc = custom_target(
        input : 'libmuslpolyfill.pc.in',
        output : 'libmuslpolyfill.pc',
        command : [jinja2_cmdline, '@INPUT@', '@OUTPUT@'],
        install : pkgconfiglibdir != 'no',
        install_dir : pkgconfiglibdir)

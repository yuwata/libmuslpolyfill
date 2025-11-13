# SPDX-License-Identifier: LGPL-2.1-or-later

project('systemd', 'c',
        version : files('meson.version'),
        license : 'LGPLv2+',
        default_options: [
                'c_std=gnu17',
                'prefix=/usr',
                'warning_level=2',
        ],
        meson_version : '>= 1.3.0',
)

conf = configuration_data()

#####################################################################
# version

project_major_version = meson.project_version().split('.')[0].split('~')[0]
if meson.project_version().contains('.')
        project_minor_version = meson.project_version().split('.')[-1].split('~')[0]
else
        project_minor_version = '0'
endif

conf.set_quoted('PROJECT_URL', 'https://github.com/yuwata/libmuslpolyfill')
conf.set('PROJECT_VERSION', project_major_version,
         description : 'Numerical project version (used where a simple number is expected)')

#####################################################################
# paths

fs = import('fs')
prefixdir = get_option('prefix')
if not prefixdir.startswith('/')
        error(f'Prefix is not absolute: "@prefixdir@"')
endif

libdir = prefixdir / get_option('libdir')
includedir = prefixdir / get_option('includedir')
datadir = prefixdir / get_option('datadir')

pkgconfiglibdir = get_option('pkgconfiglibdir')
if pkgconfiglibdir == ''
        pkgconfiglibdir = libdir / 'pkgconfig'
endif

docdir = get_option('docdir')
if docdir == ''
        docdir = datadir / 'doc/libmuslpolyfill'
endif


conf.set_quoted('PREFIX',                                     prefixdir)
conf.set_quoted('INCLUDE_DIR',                                includedir)
conf.set_quoted('LIBDIR',                                     libdir)

#####################################################################
# compiler

cc = meson.get_compiler('c')
userspace_c_args = []
userspace_c_ld_args = []
userspace_sources = []

# Those generate many false positives, and we do not want to change the code to
# avoid them.
basic_disabled_warnings = [
        '-Wno-missing-field-initializers',
        '-Wno-unknown-warning-option',
        '-Wno-unused-parameter',
        '-Wno-nonnull-compare',
]

possible_common_cc_flags = [
        '-Warray-bounds',     # clang
        '-Warray-bounds=2',
        '-Wdate-time',
        '-Wendif-labels',
        '-Werror=bool-compare',
        '-Werror=discarded-qualifiers',
        '-Werror=flex-array-member-not-at-end',
        '-Werror=format=2',
        '-Werror=format-signedness',
        '-Werror=implicit-function-declaration',
        '-Werror=implicit-int',
        '-Werror=incompatible-pointer-types',
        '-Werror=int-conversion',
        '-Werror=missing-declarations',
        '-Werror=missing-parameter-name',
        '-Werror=missing-prototypes',
        '-Werror=overflow',
        '-Werror=override-init',
        '-Werror=pointer-sign',
        '-Werror=return-type',
        '-Werror=sequence-point',
        '-Werror=shift-count-overflow',
        '-Werror=shift-overflow=2',
        '-Werror=strict-flex-arrays',
        '-Werror=undef',
        '-Wfloat-equal',
        # gperf prevents us from enabling this because it does not emit fallthrough
        # attribute with clang.
        #'-Wimplicit-fallthrough',
        '-Wimplicit-fallthrough=5',
        '-Winit-self',
        '-Wlogical-op',
        '-Wmissing-include-dirs',
        '-Wmissing-noreturn',
        '-Wnested-externs',
        '-Wold-style-definition',
        '-Wpointer-arith',
        '-Wredundant-decls',
        '-Wshadow',
        '-Wstrict-aliasing=2',
        '-Wstrict-prototypes',
        '-Wsuggest-attribute=noreturn',
        '-Wunterminated-string-initialization',
        '-Wunused-function',
        '-Wwrite-strings',
        '-Wzero-as-null-pointer-constant',
        '-Wzero-length-bounds',

        # negative arguments are correctly detected starting with meson 0.46.
        '-Wno-error=#warnings',  # clang
        '-Wno-string-plus-int',  # clang

        '-fdiagnostics-show-option',
        '-fno-common',
        '-fstack-protector',
        '-fstack-protector-strong',
        '-fstrict-flex-arrays=3',
        '--param=ssp-buffer-size=4',
]

possible_common_link_flags = [
        '-fstack-protector',
]

c_args = get_option('c_args')

# Disable -Wmaybe-uninitialized when compiling with -Os/-O1/-O3/etc. There are
# too many false positives with gcc >= 8. Effectively, we only test with -O0
# and -O2; this should be enough to catch most important cases without too much
# busywork. See https://github.com/systemd/systemd/pull/19226.
if cc.get_id() == 'gcc' and (not '02'.contains(get_option('optimization')) or
                             cc.version().version_compare('<10') or
                             '-Os' in c_args or
                             '-O1' in c_args or
                             '-O3' in c_args or
                             '-Og' in c_args or
                             '-Ofast' in c_args)
        possible_common_cc_flags += '-Wno-maybe-uninitialized'
endif

# Disable -Wno-unused-result with gcc, see
# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66425.
if cc.get_id() == 'gcc'
        possible_common_cc_flags += '-Wno-unused-result'
endif

# --as-needed and --no-undefined are provided by meson by default,
# run 'meson configure' to see what is enabled
possible_link_flags = [
        '-Wl,--fatal-warnings',
        '-Wl,-z,now',
        '-Wl,-z,relro',
        '-Wl,-z,gcs-report-dynamic=none',
        '-Wl,--gc-sections',
]

possible_cc_flags = [
        '-fno-omit-frame-pointer',
        '-fno-strict-aliasing',
        '-fstrict-flex-arrays=1',
        '-fvisibility=hidden',
]

add_project_arguments(
        cc.get_supported_arguments(
                basic_disabled_warnings,
                possible_common_cc_flags
        ),
        language : 'c')

add_project_link_arguments(
        cc.get_supported_link_arguments(possible_common_link_flags),
        language : 'c')

userspace_c_args += cc.get_supported_arguments(possible_cc_flags)
userspace_c_ld_args += cc.get_supported_link_arguments(possible_link_flags)

if cc.compiles('''
   #include <time.h>
   #include <inttypes.h>
   typedef uint64_t usec_t;
   usec_t now(clockid_t clock);
   int main(void) {
           struct timespec now;
           return 0;
   }
''', args: '-Werror=shadow', name : '-Werror=shadow with local shadowing')
        add_project_arguments('-Werror=shadow', language : 'c')
endif

cpp = ' '.join(cc.cmd_array() + get_option('c_args')) + ' -E'

#####################################################################
# constants

conf.set('_GNU_SOURCE', 1)
conf.set('__SANE_USERSPACE_TYPES__', true)
conf.set('_LARGEFILE64_SOURCE', 1)

#####################################################################
# config.h

config_h = configure_file(
        output : 'config.h',
        configuration : conf)

userspace_c_args += ['-include', 'config.h']

userspace = declare_dependency(
        compile_args : userspace_c_args,
        link_args : userspace_c_ld_args,
)

#####################################################################
# tools

pymod = import('python')
python = pymod.find_installation('python3', required : true, modules : ['jinja2'])
meson_render_jinja2 = files('tools/meson-render-jinja2.py')
jinja2_cmdline = [meson_render_jinja2, config_h]

#####################################################################
# install

install_data('LICENSE',
             install_dir : docdir)

#####################################################################
# summary

summary({
        'prefix directory'                          : prefixdir,
        'include directory'                         : includedir,
        'lib directory'                             : libdir,
})

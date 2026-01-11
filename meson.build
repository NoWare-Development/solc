project('solc', 'c')

flags = []

if get_option('buildtype') == 'debug'
  flags += '-D_DEBUG'
endif

libsolc_src = []
libsolc_dep = []
libsolc_inc = include_directories('include')
libsolc_dir = 'libsolc'
subdir(libsolc_dir)

libsolc_lib = shared_library(
  'solc',
  libsolc_src,
  dependencies: libsolc_dep,
  include_directories: [ libsolc_inc, include_directories(libsolc_dir) ],
  cpp_args: [ flags ],
)

libsolc = declare_dependency(
  link_with: libsolc_lib,
  include_directories: libsolc_inc,
)

solc_src = []
solc_dep = []
solc_inc = include_directories('src')
subdir('src')

solc_exe = executable(
  'solc',
  solc_src,
  dependencies: libsolc,
  include_directories: solc_inc,
  cpp_args: [ flags ],
)

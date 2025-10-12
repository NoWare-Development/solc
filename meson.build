project('solc', 'cpp')

flags = []

if get_option('buildtype') == 'debug'
  flags += '-D_DEBUG'
endif

libsolc_src = []
libsolc_dep = [ dependency('llvm') ]
libsolc_inc = include_directories('include')
subdir('libsolc')

libsolc_lib = shared_library(
  'solc',
  libsolc_src,
  dependencies: libsolc_dep,
  include_directories: libsolc_inc,
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

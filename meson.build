project('nlc', 'cpp')

libnlc_src = []
libnlc_dep = [ dependency('llvm') ]
libnlc_inc = include_directories('include')
subdir('libnlc')

libnlc_lib = shared_library(
  'nlc',
  libnlc_src,
  dependencies: libnlc_dep,
  include_directories: libnlc_inc,
  cpp_args: [ '-D_GLIBCXX_DEBUG' ],
)

libnlc = declare_dependency(
  link_with: libnlc_lib,
  include_directories: libnlc_inc,
)

nlc_src = []
nlc_dep = []
nlc_inc = include_directories('src')
subdir('src')

nlc_exe = executable(
  'nlc',
  nlc_src,
  dependencies: libnlc,
  include_directories: nlc_inc,
  cpp_args: [ '-D_GLIBCXX_DEBUG' ],
)

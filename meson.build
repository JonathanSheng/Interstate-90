project(
  'interstate90',
  'cpp',
  version: '0.1.0',
  default_options: [
    'cpp_std=c++20',
    'warning_level=2',
  ],
)

dpdk = dependency('libdpdk')

executable(
  'interstate90',
  'app/main.cpp',
  dependencies: dpdk,
)

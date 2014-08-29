cpp-dev:
  pkg.installed:
    - pkgs:
      - gcc
      - g++
      - gdb
      - cmake
      - make
      - libtool
      - pkg-config
      - colorgcc
      - cppcheck

cpp-clang-dev:
  pkg.installed:
    - pkgs:
      - llvm
      - clang
      - libclang-dev

FROM fedora:latest
RUN dnf makecache --refresh && dnf -y update && dnf -y install gcc g++ gdb gtest gtest-devel cmake clang-tools-extra doxygen
# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/maejimafumika/esp/esp-idf/components/bootloader/subproject"
  "/Users/maejimafumika/Desktop/Lab/research/playroom2023/c_bob/build/bootloader"
  "/Users/maejimafumika/Desktop/Lab/research/playroom2023/c_bob/build/bootloader-prefix"
  "/Users/maejimafumika/Desktop/Lab/research/playroom2023/c_bob/build/bootloader-prefix/tmp"
  "/Users/maejimafumika/Desktop/Lab/research/playroom2023/c_bob/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/maejimafumika/Desktop/Lab/research/playroom2023/c_bob/build/bootloader-prefix/src"
  "/Users/maejimafumika/Desktop/Lab/research/playroom2023/c_bob/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/maejimafumika/Desktop/Lab/research/playroom2023/c_bob/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()

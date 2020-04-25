cmake_minimum_required(VERSION 3.11)


set(TEST_LAB3_BINS
    "badinsn"
    "badinsn2"
    "hello"
    "testputc"
    "testcreatepmo"
    "testmappmo"
    "testmappmoerr"
    "testsbrk"
    "faultread"
    "faultwrite"
    "testpf"
)

foreach(bin ${TEST_LAB3_BINS})
  file(GLOB ${bin}_source_files "${bin}.c" "../lib/*.c" "../lib/*.S")
  add_executable(${bin}.bin ${${bin}_source_files})
  set_property(
          TARGET ${bin}.bin
          APPEND_STRING
          PROPERTY
          LINK_FLAGS
          "-e START"
  )
endforeach(bin)

include_directories("../")
include_directories("../../")

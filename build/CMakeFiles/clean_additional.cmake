# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "src/CMakeFiles/chatqtp_autogen.dir/AutogenUsed.txt"
  "src/CMakeFiles/chatqtp_autogen.dir/ParseCache.txt"
  "src/chatqtp_autogen"
  )
endif()

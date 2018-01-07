cmake_minimum_required(VERSION 2.8.11)
if(POLICY CMP0043)
  cmake_policy(SET CMP0043 NEW)
endif()
project(EasyARSamples C CXX)
set(exe_output_path ${CMAKE_BINARY_DIR}/bin)

macro(detect_EasyAR)
  set(_DIR ${EasyAR_DIR})
  unset(EasyAR_DIR CACHE)
  unset(EasyAR_FOUND)

  find_path(EasyAR_DIR
    NAMES include/easyar/types.h
    PATHS ${_DIR} ${ARGN}
    DOC "EasyAR SDK Root Dir"
    NO_DEFAULT_PATH
    NO_CMAKE_FIND_ROOT_PATH
  )
  if(EasyAR_DIR)
    set(EasyAR_FOUND 1)
    set(_ARCH)
    if(CMAKE_CL_64)
      set(_ARCH x64)
    else()
      set(_ARCH x86)
    endif()
    set(EasyAR_INCLUDE_DIRS ${EasyAR_DIR}/include)
    set(EasyAR_LIBS ${EasyAR_DIR}/Windows/${_ARCH}/lib/EasyAR.lib)
    set(EasyAR_RUNTIMES ${EasyAR_DIR}/Windows/${_ARCH}/bin/EasyAR.dll)
    foreach(f ${EasyAR_LIBS} ${EasyAR_RUNTIMES})
      if(NOT EXISTS ${f})
        set(EasyAR_FOUND 0)
      endif()
    endforeach()
  endif()
  if(NOT EasyAR_FOUND)
    message(SEND_ERROR "missing EasyAR")
  endif()
endmacro()

macro(detect_Qt5)
  foreach(c ${ARGN})
    find_package(Qt5${c} QUIET)
    if(NOT Qt5${c}_FOUND)
      message(SEND_ERROR "missing Qt5${c}")
    endif()
  endforeach()
endmacro()

include(FindOpenGL)
set(qt5_components Core Gui OpenGL Widgets)
detect_Qt5(${qt5_components})
detect_EasyAR(.)

set(tgt_name eMPTYHOUSE-AR)
project(${tgt_name})

file(GLOB srcs *.c*)
file(GLOB headers *.h*)
file(GLOB srcs_utility utility/*.c*)
file(GLOB headers_utility utility/*.h*)
source_group("utility" FILES ${srcs_utility} ${headers_utility})
list(APPEND srcs ${srcs_utility})
list(APPEND headers ${headers_utility})
file(GLOB srcs_glew glew/src/*.c*)
source_group("glew" FILES ${srcs_glew})
list(APPEND srcs ${srcs_glew})
add_definitions(-DGLEW_STATIC)

if(Qt5Core_FOUND)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
  file(GLOB _RCCS_FILES "*.qrc")
  file(GLOB _UI_FILES "*.ui")
  unset(_UI_OUTFILES)
  unset(_RCC_OUTFILES)
  source_group("res" FILES ${_RCCS_FILES} ${_UI_FILES})

  if(_RCCS_FILES)
    qt5_add_resources(_RCC_OUTFILES ${_RCCS_FILES})
  endif()
  if(_UI_FILES)
    qt5_wrap_ui(_UI_OUTFILES ${_UI_FILES})
  endif()

  foreach(f ${_RCC_OUTFILES} ${_UI_OUTFILES})
    list(APPEND srcs ${f})
    source_group(gen FILES ${f})
  endforeach()
  source_group("gen" FILES ${CMAKE_CURRENT_BINARY_DIR}/${tgt_name}_automoc.cpp ${CMAKE_CURRENT_BINARY_DIR}/${tgt_name}_automoc.cxx)
endif()

if(WIN32)
  set(WIN_GUI WIN32)
elseif(APPLE)
  set(WIN_GUI MACOSX_BUNDLE)
endif()
add_executable(${tgt_name} ${WIN_GUI} ${srcs} ${headers})
if(Qt5Core_FOUND)
  qt5_use_modules(${tgt_name} ${qt5_components})
endif()

target_include_directories(${tgt_name} PUBLIC ${EasyAR_INCLUDE_DIRS} glew/include)
target_link_libraries(${tgt_name} ${EasyAR_LIBS} ${OPENGL_gl_LIBRARY})
set_target_properties(${tgt_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${exe_output_path}")
foreach(dll ${EasyAR_RUNTIMES})
  get_filename_component(dll_name ${dll} NAME)
  add_custom_command(TARGET ${tgt_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS    -E copy_if_different "${dll}" "${exe_output_path}/${CMAKE_CFG_INTDIR}/${dll_name}"
    DEPENDS ${dll}
    COMMENT "copying ${dll_name}"
  )
  add_custom_command(TARGET ${tgt_name}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS    -E copy_directory "${EasyAR_DIR}/assets" "${exe_output_path}/${CMAKE_CFG_INTDIR}"
    DEPENDS "${EasyAR_DIR}/assets"
    COMMENT "copying assets"
  )
endforeach()

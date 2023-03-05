cmake_minimum_required(VERSION 3.23)

set(SOURCES_CPP
  # mainfiles
  src/main.cpp

  # Render
  src/render/vulkan/allocator.cpp
  src/render/vulkan/buffer.cpp
  src/render/vulkan/command_pool.cpp
  src/render/vulkan/descriptor_pool.cpp
  src/render/vulkan/device.cpp
  src/render/vulkan/image.cpp
  src/render/vulkan/instance.cpp
  src/render/vulkan/pipeline.cpp
  src/render/vulkan/render_pass.cpp
  src/render/vulkan/swapchain.cpp
  src/render/vulkan/includes.cpp
  src/render/gpu_structs.cpp
  src/render/renderer.cpp
  src/render/recorder.cpp
  src/render/render_structs.cpp
  src/render/window.cpp

  # World
  src/world/world.cpp
)

add_executable(Dynamo ${SOURCES_CPP})

target_include_directories(Dynamo PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(Dynamo PUBLIC ${CMAKE_SOURCE_DIR}/inc)

project(Dynamo VERSION 0.0.0.1) # MAJOR.MINOR.PATCH.TWEAK
target_compile_definitions(Dynamo PUBLIC VERSION_MAJOR=${PROJECT_VERSION_MAJOR})
target_compile_definitions(Dynamo PUBLIC VERSION_MINOR=${PROJECT_VERSION_MINOR})
target_compile_definitions(Dynamo PUBLIC VERSION_PATCH=${PROJECT_VERSION_PATCH})
target_compile_definitions(Dynamo PUBLIC VERSION_TWEAK=${PROJECT_VERSION_TWEAK})

# Compiler flags
target_compile_definitions(Dynamo PUBLIC _GLIBCXX_ASSERTIONS)
target_compile_options(Dynamo PUBLIC -std=c++2b)
# target_compile_options(Dynamo PUBLIC "$<$<CONFIG:Debug>:-Og>")
# target_compile_options(Dynamo PUBLIC -lasan)
# target_compile_options(Dynamo PUBLIC -fno-omit-frame-pointer)
target_compile_options(Dynamo PUBLIC -Og)
target_compile_options(Dynamo PUBLIC -march=native)
target_compile_options(Dynamo PUBLIC -fvisibility=default)
target_compile_options(Dynamo PUBLIC -Wfatal-errors)

target_compile_options(Dynamo PUBLIC -D_GLIBCXX_DEBUG)
target_compile_options(Dynamo PUBLIC -D_GLIBCXX_ASSERTIONS)
target_compile_options(Dynamo PUBLIC -D_GLIBCXX_CONCEPT_CHECKS)
# target_compile_options(Dynamo PUBLIC -D_GLIBCXX_DEBUG_BACKTRACE)
# target_compile_options(Dynamo PUBLIC --enable-libstdcxx-backtrace=yes)
# target_compile_options(Dynamo PUBLIC -lstdc++_libbacktrace)



target_compile_options(Dynamo PUBLIC -Wall)
target_compile_options(Dynamo PUBLIC -Wextra)
target_compile_options(Dynamo PUBLIC -Wpedantic)
target_compile_options(Dynamo PUBLIC -Wunused)
target_compile_options(Dynamo PUBLIC -Wuninitialized)
target_compile_options(Dynamo PUBLIC -pedantic-errors)
target_compile_options(Dynamo PUBLIC -Wshadow )
target_compile_options(Dynamo PUBLIC -Wpointer-arith )
target_compile_options(Dynamo PUBLIC -Wcast-qual)
target_compile_options(Dynamo PUBLIC -Wredundant-move)
target_compile_options(Dynamo PUBLIC -Wpessimizing-move)
target_compile_options(Dynamo PUBLIC -Wuseless-cast)
target_compile_options(Dynamo PUBLIC -Wconversion)
target_compile_options(Dynamo PUBLIC -Wdouble-promotion)
target_compile_options(Dynamo PUBLIC -Wnonnull)
target_compile_options(Dynamo PUBLIC -Wnonnull-compare)
target_compile_options(Dynamo PUBLIC -Wnull-dereference)
target_compile_options(Dynamo PUBLIC -Winfinite-recursion)
target_compile_options(Dynamo PUBLIC -Wimplicit-fallthrough)
target_compile_options(Dynamo PUBLIC -Wignored-qualifiers)
target_compile_options(Dynamo PUBLIC -Wmissing-include-dirs)
# target_compile_options(Dynamo PUBLIC -Wsuggest-attribute=pure)
# target_compile_options(Dynamo PUBLIC -Wsuggest-attribute=const)
target_compile_options(Dynamo PUBLIC -Wsuggest-attribute=noreturn)
target_compile_options(Dynamo PUBLIC -Wmissing-noreturn)
# target_compile_options(Dynamo PUBLIC -Wsuggest-attribute=malloc)
# target_compile_options(Dynamo PUBLIC -Wsuggest-attribute=format)
target_compile_options(Dynamo PUBLIC -Wmissing-format-attribute)
target_compile_options(Dynamo PUBLIC -Wsuggest-attribute=cold)
target_compile_options(Dynamo PUBLIC -Walloc-zero)
target_compile_options(Dynamo PUBLIC -Warith-conversion)
# target_compile_options(Dynamo PUBLIC -Wduplicated-branches)
target_compile_options(Dynamo PUBLIC -Wduplicated-cond)
target_compile_options(Dynamo PUBLIC -Wtrampolines)
target_compile_options(Dynamo PUBLIC -Wfloat-equal)
target_compile_options(Dynamo PUBLIC -Wunsafe-loop-optimizations)
target_compile_options(Dynamo PUBLIC -Wcast-qual)
target_compile_options(Dynamo PUBLIC -Wcast-align)
target_compile_options(Dynamo PUBLIC -Wdangling-else)
target_compile_options(Dynamo PUBLIC -Wvla)
target_compile_options(Dynamo PUBLIC -Wparentheses)
target_compile_options(Dynamo PUBLIC -Wempty-body)
target_compile_options(Dynamo PUBLIC -Wlogical-op)
target_compile_options(Dynamo PUBLIC -Wmissing-field-initializers)
target_compile_options(Dynamo PUBLIC -Wpacked)
target_compile_options(Dynamo PUBLIC -Wredundant-decls)
target_compile_options(Dynamo PUBLIC -Wdisabled-optimization)

set(CMAKE_EXPORT_COMPILE_COMMANDS)

# Add fmt
add_library(fmt STATIC IMPORTED)
set_target_properties(fmt PROPERTIES
  IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libfmt.a
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/inc/fmt/include
)
target_link_libraries(Dynamo fmt)

# Add vkfw_glfw
add_library(vkfw STATIC IMPORTED)
set_target_properties(vkfw PROPERTIES
  IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/lib/libglfw3.a
  INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/inc/
)
target_link_libraries(Dynamo vkfw)

# Add Vulkan 
add_library(vulkan INTERFACE)
target_include_directories(vulkan INTERFACE ${CMAKE_SOURCE_DIR}/inc/vulkan/include)
target_link_libraries(Dynamo vulkan)
if(APPLE) 
  target_link_libraries(Dynamo "-framework Cocoa -framework IOKit")
endif()

# Add VMA
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE ${CMAKE_SOURCE_DIR}/inc/vma/include)
target_link_libraries(Dynamo vma)

# Add tinyobjloader
add_library(tinyobjloader INTERFACE)
target_include_directories(tinyobjloader INTERFACE ${CMAKE_SOURCE_DIR}/inc/tinyobjloader)
target_link_libraries(Dynamo tinyobjloader)

# Add stb
add_library(stb INTERFACE)
target_include_directories(stb INTERFACE ${CMAKE_SOURCE_DIR}/inc/stb)
target_link_libraries(Dynamo stb)

# Add sebib
add_library(sebib INTERFACE)
target_include_directories(sebib INTERFACE ${CMAKE_SOURCE_DIR}/inc/)
target_link_libraries(Dynamo sebib)

# Add glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE ${CMAKE_SOURCE_DIR}/inc/glm)
target_link_libraries(Dynamo glm)


# Compile shaders function Stack overflow #60420700
find_package(Vulkan COMPONENTS glslc)
find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

function(compile_shader target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "ENV;FORMAT" "SOURCES")
    foreach(source ${arg_SOURCES})
        add_custom_command(
            OUTPUT ${source}.${arg_FORMAT}
            DEPENDS ${source}
            DEPFILE ${source}.d
            COMMAND
                ${glslc_executable}
                $<$<BOOL:${arg_ENV}>:--target-env=${arg_ENV}>
                $<$<BOOL:${arg_FORMAT}>:-mfmt=${arg_FORMAT}>
                -MD -MF ${source}.d
                -o ${source}.${arg_FORMAT}
                ${CMAKE_CURRENT_SOURCE_DIR}/${source}
        )
        target_sources(${target} PRIVATE ${source}.${arg_FORMAT})
    endforeach()
endfunction()

compile_shader(Dynamo
  ENV vulkan1.0
  FORMAT bin
  SOURCES
    src/render/shaders/shader.vert
    src/render/shaders/shader.frag
)
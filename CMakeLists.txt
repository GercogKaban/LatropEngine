cmake_minimum_required (VERSION 3.20)
     
project (LatropGame CXX)

include(FetchContent)

set(CMAKE_SKIP_INSTALL_RULES ON)
set(CMAKE_SUPPRESS_REGENERATION ON)

file(GLOB SOURCE_FILES src/*.h src/*.cpp)

find_package(Vulkan)

add_subdirectory(LizardGraphics-2.0)
add_subdirectory(LatropPhysics)

add_executable(LatropGame ${SOURCE_FILES})

if(WIN32)
set_target_properties(LatropGame PROPERTIES VS_PLATFORM_TOOLSET ClangCL)
endif()

if(WIN32)
    set(PYTHON_COMMAND python)
else()
    set(PYTHON_COMMAND python3)
endif()

add_custom_target(move_textures ALL 
    COMMAND ${CMAKE_COMMAND} -E env PROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR} ${PYTHON_COMMAND} ${PROJECT_SOURCE_DIR}/move_textures.py ${PROJECT_SOURCE_DIR}/LizardGraphics-2.0 ${CMAKE_BINARY_DIR}
    COMMENT "Running script before build"
)

add_dependencies(LatropGame move_textures)

target_compile_features(LatropGame PRIVATE cxx_std_20)
target_link_libraries(LatropGame LizardGraphics ${Vulkan_LIBRARIES} glfw LatropPhysics TracyClient)

add_compile_definitions(TRACY_ENABLE)
add_compile_definitions(TRACY_ON_DEMAND)
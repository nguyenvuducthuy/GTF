cmake_minimum_required(VERSION 3.5)

project(GTF)

#set( GTF_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin )

set (CMAKE_CXX_STANDARD 11)

if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
    set( GTF_ARCH x64 )
else() 
    set( GTF_ARCH x86 )
endif() 

set( GTF_OUTPUT_DIR bin )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GTF_OUTPUT_DIR} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${GTF_OUTPUT_DIR} )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${GTF_OUTPUT_DIR} )

# Second, for multi-config builds (e.g. msvc)
foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${GTF_OUTPUT_DIR} )
    set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${GTF_OUTPUT_DIR} )
    set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${GTF_OUTPUT_DIR} )
endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")	
	SET (CMAKE_C_COMPILER             "/usr/bin/clang")
    SET (CMAKE_C_FLAGS                "-Wall -std=c99")
    SET (CMAKE_C_FLAGS_DEBUG          "-g")
    SET (CMAKE_C_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")
    SET (CMAKE_CXX_FLAGS                "-Wall -Wc++11-extensions -std=c++11")
    SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
    SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
    SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")

    SET (CMAKE_AR      "/usr/bin/ar")
    SET (CMAKE_LINKER  "/usr/bin/ld")
    SET (CMAKE_NM      "/usr/bin/nm")
    SET (CMAKE_OBJDUMP "/usr/bin/objdump")
    SET (CMAKE_RANLIB  "/usr/bin/ranlib")

	add_definitions(-D_GLFW_COCOA)

	#copy depencencies
	file(COPY external/FBX/2016.1.2/lib/clang/release/libfbxsdk.dylib DESTINATION ${GTF_OUTPUT_DIR})

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	#add_definitions(-D_GLFW_LINUX)
else()	
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99 -pedantic")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -pedantic")
	add_definitions(-D_GLFW_WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)

	#copy depencencies
	file(COPY external/FBX/2016.1.2/lib/vs2015/${GTF_ARCH}/libfbxsdk.dll DESTINATION ${GTF_OUTPUT_DIR})
endif()

file(GLOB SHARED_INCLUDE_DIRS include thirdparty/glm thirdparty/imgui thirdparty/tinylibs external/glfw/include external/FBX/2016.1.2/include)

#gtf
include_directories(include thirdparty/glm thirdparty/imgui thirdparty/tinylibs external/glfw/include external/FBX/2016.1.2/include)
file(GLOB INCLUDES "include/**/*.h" "src/**/*.h")
file(GLOB SOURCES "src/**/*.c" "src/**/*.cpp"  )
add_library(gtf STATIC ${SOURCES} ${INCLUDES})
target_include_directories(gtf PUBLIC include thirdparty/glm thirdparty/imgui thirdparty/tinylibs external/glfw/include external/FBX/2016.1.2/include)

#gtf_thirdparty
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")	
	file(GLOB GLFW_SOURCES external/glfw/src/context.c external/glfw/src/init.c external/glfw/src/input.c external/glfw/src/monitor.c external/glfw/src/vulkan.c external/glfw/src/window.c external/glfw/src/posix_tls.c external/glfw/src/nsgl_context.m external/glfw/src/cocoa_init.m external/glfw/src/cocoa_joystick.m external/glfw/src/cocoa_monitor.m external/glfw/src/cocoa_time.c external/glfw/src/cocoa_window.m)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
else()	
	file(GLOB GLFW_SOURCES external/glfw/src/context.c external/glfw/src/egl_context.c external/glfw/src/init.c external/glfw/src/input.c external/glfw/src/monitor.c external/glfw/src/vulkan.c external/glfw/src/wgl_context.c external/glfw/src/win32_init.c external/glfw/src/win32_joystick.c external/glfw/src/win32_monitor.c external/glfw/src/win32_time.c external/glfw/src/win32_tls.c external/glfw/src/win32_window.c external/glfw/src/window.c)
endif()
file(GLOB INCLUDES "thirdparty/imgui/*.h" "thirdparty/tinylibs/*.h")
file(GLOB SOURCES "thirdparty/imgui/*.c" "thirdparty/imgui/*.cpp" "thirdparty/tinylibs/*.c" "thirdparty/tinylibs/*.cpp" ${GLFW_SOURCES})
add_library(gtf_thirdparty STATIC ${SOURCES} ${INCLUDES})
target_include_directories(gtf_thirdparty PUBLIC include thirdparty/glm thirdparty/imgui thirdparty/tinylibs external/glfw/include external/FBX/2016.1.2/include)

#depencendies
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")	
	find_library(CF_LIBRARY CoreFoundation)
	find_library(COCOA_LIBRARY Cocoa)
	find_library(IOKIT_LIBRARY IOKit)
	find_library(COREVIDEO_LIBRARY CoreVideo)
	find_library(OPENGL_LIBRARY OpenGL)
	find_library(APPKIT_LIBRARY AppKit)
	set(MACOS_LIBRARIES ${CF_LIBRARY} ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${COREVIDEO_LIBRARY} ${OPENGL_LIBRARY} ${APPKIT_LIBRARY})

	link_directories(external/FBX/2016.1.2/lib/clang/release)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
else()
	link_directories(external/FBX/2016.1.2/lib/vs2015/${GTF_ARCH})
	link_directories(external/glfw/lib-vc2015)
endif()

# Find the QtWidgets library
# find_package(Qt5 COMPONENTS
#              Core
#              Widgets
#              Gui
#              OpenGL
#              Network
#              )

# set(CMAKE_AUTOMOC ON)

#examples
set( GTF_EXAMPLES CalcGraph NoiseGenerator MeshViewer RigGraph)

foreach( EXAMPLE ${GTF_EXAMPLES} )
	file(GLOB INCLUDES "examples/${EXAMPLE}/*.h")
	file(GLOB SOURCES "examples/${EXAMPLE}/*.c" "examples/${EXAMPLE}/*.cpp" )
	add_executable(${EXAMPLE} ${SOURCES} ${INCLUDES})
	target_include_directories(${EXAMPLE} PUBLIC include thirdparty/glm thirdparty/imgui thirdparty/tinylibs external/glfw/include external/FBX/2016.1.2/include)

	target_link_libraries(${EXAMPLE} gtf)
	target_link_libraries(${EXAMPLE} gtf_thirdparty)

	# target_compile_options(${EXAMPLE}
	#   PRIVATE
	#     $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4127 /EHsc>
	#     $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
	#     $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
	# )
	# target_link_libraries(${EXAMPLE}
	#     Qt5::Core
	#     Qt5::Widgets
	#     Qt5::Gui
	#     Qt5::OpenGL
	#     Qt5::Network
	# )

	# target_compile_definitions(${EXAMPLE}
	#   PUBLIC
	#     ${Qt5Widgets_DEFINITIONS}
	# )

	# target_compile_features(${EXAMPLE}
	#   PUBLIC
	#     cxx_generic_lambdas # Require C++14
	# )

	# qt5_wrap_cpp(nodes_moc
 #    ${HEADERS_TO_MOC}
	#   TARGET ${EXAMPLE}
	#   OPTIONS --no-notes # Don't display a note for the headers which don't produce a moc_*.cpp
	# )

	# target_sources(${EXAMPLE} PRIVATE ${nodes_moc})

	if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")	
		target_link_libraries(${EXAMPLE} ${MACOS_LIBRARIES})
		target_link_libraries(${EXAMPLE} fbxsdk)
	elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	else()	
		target_link_libraries(${EXAMPLE} libfbxsdk)
		target_link_libraries(${EXAMPLE} glfw3)
		target_link_libraries(${EXAMPLE} opengl32)
	endif()
endforeach()


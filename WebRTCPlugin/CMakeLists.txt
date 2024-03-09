add_executable(WebRTCLib)

target_sources(
        WebRTCLib
        PRIVATE
        main.cpp
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Visual Studio Generator has not been supported supression warnings of
    # external headers yet. https://gitlab.kitware.com/cmake/cmake/-/issues/17904
    if(CMAKE_GENERATOR MATCHES "Visual Studio")
        set(VISUALSTUDIO_GENERATOR 1)
    else()
        set(VISUALSTUDIO_GENERATOR 0)
    endif()

    target_compile_options(
            WebRTCLib
            PUBLIC $<$<NOT:${VISUALSTUDIO_GENERATOR}>:-Werror>
            $<${VISUALSTUDIO_GENERATOR}:-Wno-c++98-compat>
            $<${VISUALSTUDIO_GENERATOR}:-Wno-c++98-compat-pedantic>
            -Wall
            -Wextra
            -Wformat=2
            -Wno-unused-parameter)
endif()


if(Windows)
#    target_precompile_headers(WebRTCLib PRIVATE pch.h)
    # Force to always compile with W4
    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
            string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
        endif()
    endif()

    # Replace /GR option to disable RTTI
    foreach(flag_var
            CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
        string(REPLACE /GR /GR- "${flag_var}" "${${flag_var}}")
    endforeach()
endif()

target_compile_definitions(WebRTCLib PUBLIC WEBRTC_WIN NOMINMAX
        VK_NO_PROTOTYPES)

target_link_libraries(
        WebRTCLib
        PRIVATE
        ${WEBRTC_LIBRARY}
#        ${Vulkan_LIBRARY}
#        ${CUDA_CUDA_LIBRARY}
#        ${NVCODEC_LIBRARIES}
        d3d11
        winmm
        Secur32
        Iphlpapi
        Msdmo
        Dmoguids
        wmcodecdspuuid
        Strmiids)

target_include_directories(
        WebRTCLib SYSTEM PUBLIC
        ${WEBRTC_INCLUDE_DIR})
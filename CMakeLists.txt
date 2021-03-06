cmake_minimum_required(VERSION 2.6)
project(corbasim C CXX)

# Numero de version
set(${PROJECT_NAME}_MAJOR_VERSION 1)
set(${PROJECT_NAME}_MINOR_VERSION 0)
set(${PROJECT_NAME}_PATCH_LEVEL 0)
set(${PROJECT_NAME}_VERSION
        "${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}")

# set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Compilación en modo Release por defecto
if(NOT DEFINED CMAKE_BUILD_TYPE OR "${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH true)
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT DEFINED CORBASIM_ORBIMPL)
    set(CORBASIM_ORBIMPL "TAO")
endif()

if("${CORBASIM_ORBIMPL}" STREQUAL "TAO")
    include(check_ace_tao)
    set(CORBASIM_ORB_LIBS
        ${ACE_LIBRARY}
        ${TAO_LIBRARY}
        ${TAO_PortableServer_LIB}
        ${TAO_AnyTypeCode_LIB}
        ${TAO_CosNaming_LIB}
        ${TAO_CosNaming_Skel_LIB}
        ${TAO_ObjRefTemplate_LIB})
    set(CORBASIM_ORB_INCLUDE_DIRS
        ${ACE_INCLUDE_DIR}
        ${TAO_INCLUDE_DIR}
        ${TAO_INCLUDE_DIR}/orbsvcs)
    set(CORBASIM_ORB_LIBRARY_DIRS
        ${TAO_LIBRARY_DIR})
    set(CORBASIM_ORB_IDL_COMPILER ${TAO_IDL_COMPILER})
    set(CORBASIM_COSNAMING_LIBS
        ${CORBASIM_ORB_LIBS}
        ${TAO_CosNaming_Serv_LIB} 
        ${TAO_ImR_Client_LIB} 
        ${TAO_Svc_Utils_LIB}
        ${TAO_Messaging_LIB} ${TAO_PI_LIB} 
        ${TAO_CodecFactory_LIB} 
        ${TAO_Valuetype_LIB} 
        ${TAO_IORTable_LIB}
    )

    if (TAO_FOUND)
        include_directories(${ACE_INCLUDE_DIR} ${TAO_INCLUDE_DIR})
    endif()
elseif("${CORBASIM_ORBIMPL}" STREQUAL "OMNIORB4")
    # TODO check omni
    set(CORBASIM_ORB_LIBS
        omniCodeSets4
        omniConnectionMgmt4
        omniDynamic4
        omniORB4
        omnisslTP4
        omnithread)
    set(CORBASIM_ORB_IDL_COMPILER omniidl)
    set(CORBASIM_ORB_IDL_COMPILER_OPTIONS -bcxx -Wba)
endif()

#add_definitions(-Wall)
add_definitions(-DCORBASIM_USE_QTSCRIPT)

#
# Dependencies
#

if(UNIX)
    set(Boost_USE_MULTITHREADED OFF)
endif()
find_package(Boost 1.45.0)

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
else(NOT Boost_FOUND)
    message(STATUS "Boost found!")
    message(STATUS "Boost library: ${Boost_LIBRARY_DIRS}")
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})

    message(STATUS "Boost include directory: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost libraries directory: ${Boost_LIBRARY_DIRS}")
endif(NOT Boost_FOUND)

find_package(Qt4)
find_package(Qwt6)
find_package(Boost 1.45.0 
    COMPONENTS system thread program_options)

message(STATUS "Boost libraries: ${Boost_LIBRARIES}")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(QT_FOUND)
    set(QT_USE_QTSCRIPT 1)
    set(QT_USE_QTSCRIPTTOOLS 1)
    include(${QT_USE_FILE})
endif()

if(NOT Qwt6_FOUND)
    message(STATUS "Qwt6 not found! Disabling plotting support!")
else()
    message(STATUS "Qwt6 available at: ${Qwt6_INCLUDE_DIR}")
endif()

# Libraries for clients
set(CORBASIM_CLIENT_LIBS 
    corbasim corbasim_qt corbasim_gui ${Boost_PROGRAM_OPTIONS_LIBRARY})
set(CORBASIM_CLIENT_STATIC_LIBS 
    corbasim_gui_s corbasim_qt_s corbasim_s ${Boost_PROGRAM_OPTIONS_LIBRARY})
# Libraries for servers
set(CORBASIM_SERVER_LIBS 
    corbasim ${Boost_PROGRAM_OPTIONS_LIBRARY})

add_subdirectory(corbasim)
add_subdirectory(corbasim_idl)

if(QT_FOUND)
    add_subdirectory(corbasim_app)
endif()

add_subdirectory(examples EXCLUDE_FROM_ALL)
add_subdirectory(test EXCLUDE_FROM_ALL)

if (EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/private 
        AND IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/private)
    add_subdirectory(private EXCLUDE_FROM_ALL)
endif()

# Opciones de empaquetado
set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_VERSION_MAJOR ${${PROJECT_NAME}_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${${PROJECT_NAME}_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_PATCH_LEVEL})
set(CPACK_SOURCE_GENERATOR "TGZ")
set(CPACK_PACKAGE_FILE_NAME
    ${PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CORBASIM_ORBIMPL}-${CMAKE_BUILD_TYPE})
include(CPack)


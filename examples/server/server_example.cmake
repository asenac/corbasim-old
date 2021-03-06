

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})

set(server_example_GENERATED
	${CMAKE_CURRENT_SOURCE_DIR}/server_exampleC.h
	${CMAKE_CURRENT_SOURCE_DIR}/server_exampleC.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/server_exampleS.h
	${CMAKE_CURRENT_SOURCE_DIR}/server_exampleS.cpp)

add_custom_command(
	OUTPUT ${server_example_GENERATED}
	DEPENDS server_example.idl
	COMMAND ${CORBASIM_ORB_IDL_COMPILER} ${CORBASIM_ORB_IDL_COMPILER_OPTIONS} server_example.idl
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_library(${CORBASIM_PREFIX}server_example_idl SHARED ${server_example_GENERATED} server_example_adapted.cpp)
install(TARGETS ${CORBASIM_PREFIX}server_example_idl DESTINATION lib)

add_library(${CORBASIM_PREFIX}server_example_idl_s STATIC ${server_example_GENERATED} server_example_adapted.cpp)
install(TARGETS ${CORBASIM_PREFIX}server_example_idl_s DESTINATION lib)

	

# Corbasim reflective library	
add_library(corbasim_reflective_prueba_Iface SHARED prueba_Iface_reflective.cpp)
target_link_libraries(corbasim_reflective_prueba_Iface
	${CORBASIM_PREFIX}server_example_idl
	# CORBASIM Library
	corbasim)
install(TARGETS corbasim_reflective_prueba_Iface DESTINATION lib)

add_library(corbasim_static_reflective_prueba_Iface STATIC prueba_Iface_reflective.cpp)
install(TARGETS corbasim_static_reflective_prueba_Iface DESTINATION lib)

# Client
add_executable(${CORBASIM_PREFIX}prueba_Iface_server_ui prueba_Iface_server_ui.cpp)
if(${CORBASIM_STATIC_LINK})
    target_link_libraries(${CORBASIM_PREFIX}prueba_Iface_server_ui 
        corbasim_static_reflective_prueba_Iface
        ${CORBASIM_PREFIX}server_example_idl_s
        ${CORBASIM_CLIENT_STATIC_LIBS}
        ${CORBASIM_ORB_LIBS}
        ${QT_LIBRARIES})
else()
    target_link_libraries(${CORBASIM_PREFIX}prueba_Iface_server_ui 
        ${CORBASIM_PREFIX}server_example_idl
        corbasim_reflective_prueba_Iface
        ${CORBASIM_ORB_LIBS}
        ${QT_LIBRARIES}
        ${CORBASIM_CLIENT_LIBS})
endif()
install(TARGETS ${CORBASIM_PREFIX}prueba_Iface_server_ui DESTINATION bin)


	

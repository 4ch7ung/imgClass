cmake_minimum_required( VERSION 2.8 )
project( imgClass )

if( APPLE )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libstdc++" )
    SET( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libstdc++" )
    SET( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -stdlib=libstdc++" )
    SET( CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -stdlib=libstdc++" )
endif( APPLE )

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

set( EXECUTABLE_OUTPUT_PATH bin )
set( SOURCE_DIR src )
set( SUBPROGRAMS extractor clusterize classify svmify )

foreach( PROGRAM_NAME ${SUBPROGRAMS} )
    add_executable( ${PROGRAM_NAME} "${SOURCE_DIR}/${PROGRAM_NAME}.cpp" )
    target_link_libraries( ${PROGRAM_NAME} ${OpenCV_LIBS} )
endforeach( PROGRAM_NAME )
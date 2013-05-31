project( ECOHT-ANN )
cmake_minimum_required( VERSION 2.6 )
set ( CMAKE_BUILD_TYPE Release )
add_definitions ( -Wall )
add_definitions ( -std=c++11 )
find_package ( Qt4 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
	${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTTESTLIB_INCLUDE_DIR}
	/usr/include/qwt)
set ( ECOHT-ANN_HDRS
	Defines.hpp
	Sample.hpp
	Helpers.hpp
	Exceptions.hpp
	)

set ( ECOHT-ANN_SRCS
	main.cpp
	mainwindow.cpp
	Neuron.cpp
	Layer.cpp
	Network.cpp
	Sample.cpp
	Facade.cpp
	Preprocessor.cpp
	)

set ( ECOHT-ANN_UIS
	mainwindow.ui
	)
QT4_WRAP_UI(UIS ${ECOHT-ANN_UIS})

set ( ECOHT-ANN_MOCS
	mainwindow.hpp
	Neuron.hpp
	Layer.hpp
	Network.hpp
	Facade.hpp
	Preprocessor.hpp
	)
QT4_WRAP_CPP(MOCS ${ECOHT-ANN_MOCS})

add_executable ( ECOHT-ANN ${ECOHT-ANN_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS} )
target_link_libraries ( ECOHT-ANN  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTESTLIB_LIBRARY} qwt )

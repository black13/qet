 cmake_minimum_required(VERSION 3.9)

project(qet)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC, UIC & RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

find_package(Qt5 COMPONENTS Core Widgets Svg REQUIRED)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core Gui Sql PrintSupport Test)

  FIND_PACKAGE(Qt5Core REQUIRED)
  FIND_PACKAGE(Qt5Gui REQUIRED)
  FIND_PACKAGE(Qt5Xml REQUIRED)
  find_package(Qt5PrintSupport REQUIRED)
  #-----------------------------------------------------
  set(QT5_INSTALLED TRUE)
  # # by default only QtCore and QtGui modules are enabled
  # # other modules must be enabled like this:
  set(QT_USE_QTXML TRUE)  # needed!

# Generate code from ui files
qt5_wrap_ui(UI_HEADERS mainwindow.ui)

# Generate rules for building source files from the resources
set(SOURCES aboutqet.cpp   
contactor.cpp  
element.cpp
elementperso.cpp
main.cpp
qetapp.cpp
schemaview.cpp
conductor.cpp
del.cpp
FixedElement.cpp
entree.cpp
panelappareils.cpp
schema.cpp
terminal.cpp
)

add_executable(qet ${SOURCES})

target_link_libraries(qet Qt5::Core Qt5::PrintSupport  Qt5::Xml Qt5::Widgets Qt5::Test  Qt5::Svg)
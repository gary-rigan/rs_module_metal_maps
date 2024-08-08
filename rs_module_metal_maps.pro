CONFIG -= qt

TEMPLATE = lib
DEFINES += RS_MODULE_METAL_MAPS_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    module/metal_map_module.cpp \
    module/metal_map_module_impl.cpp \
    rs_module_metal_maps.cpp \
    rs_module_metal_maps_option.cpp

HEADERS += \
    module/metal_map_module.h \
    module/metal_map_module_impl.h \
    rs_module_metal_maps.h \
    rs_module_metal_maps_config.h \
    rs_module_metal_maps_option.h


message("*****************************Robsys module[$$TARGET]*********************************")

include(../../robsys_build_config/qt_env.pri)
include($${RSWS_BUIILD_ENV}/qmake/robsys_platform.pri)
include($${RSWS_BUIILD_ENV}/qmake/robsys_rpath.pri)

include($${RSWS_BUIILD_ENV}/modules/RobsysFindEigen.pri)
include($${RSWS_BUIILD_ENV}/modules/RobsysFindPcl.1.8.1.pri)


INCLUDEPATH += $${ROBSYS_ENV_PATH}/include/
INCLUDEPATH += $${ROBSYS_ENV_PATH}/include/utils/
INCLUDEPATH += $${ROBSYS_ENV_PATH}/include/robsys_message/
INCLUDEPATH += $${ROBSYS_ENV_PATH}/include/robsys_system/
INCLUDEPATH += $${ROBSYS_ENV_PATH}/include/parsers/
INCLUDEPATH += $${ROBSYS_ENV_PATH}/include/robsys_pclbridge

INCLUDEPATH += $${ROBSYS_EIGEN3_INCLUDE}
INCLUDEPATH += $${ROBSYS_PCL_INCLUDE}

LIB_THR_PARTY_PATH  = $${ROBSYS_PLATFORM_ENV_PATH}/libs/
LIB_INSTALL_PATH    = $${ROBSYS_PLATFORM_ENV_PATH}/yogo_lib/
HEADER_INSTALL_PATH = $${ROBSYS_ENV_PATH}/include/$$TARGET/


message("Env path            " $${ROBSYS_PLATFORM_ENV_PATH})
message("Thr party lib path  " $${LIB_THR_PARTY_PATH})
message("Lib install path    " $${LIB_INSTALL_PATH})
message("Header install path " $${HEADER_INSTALL_PATH})

LIBS += -L$${LIB_INSTALL_PATH}/ -lmap_utility -lrobsys_pclbridge
LIBS += -L$${LIB_INSTALL_PATH}/ -lutils -lparsers  -lrobsys_message -lrobsys_system




LIBS += -L$${ROBSYS_PCL_LIBRARIES}

INSTALL_HEADERS = rs_module_metal_maps.h
for(header, INSTALL_HEADERS) {
path = $${HEADER_INSTALL_PATH}/$${dirname(header)}
    eval(headers_$${path}.files += $$header)
    eval(headers_$${path}.path = $$path)
    eval(INSTALLS *= headers_$${path})
}
target.path = $${LIB_INSTALL_PATH}
INSTALLS += target
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    config/metal_maps.yaml

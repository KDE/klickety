project(klickety)


cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.3.0")

find_package(ECM 1.3.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets )
find_package(KF5 REQUIRED COMPONENTS 
    CoreAddons 
    Config 
    WidgetsAddons 
    Config 
    I18n 
    GuiAddons 
    ConfigWidgets 
    ItemViews 
    IconThemes 
    XmlGui
    KIO
    NotifyConfig
    KDELibs4Support)

find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMInstallIcons)
include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)

add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-DTRANSLATION_DOMAIN="klickety")


set(klickety_SRCS
    gamescene.cpp
    gameview.cpp
    main.cpp
    mainwindow.cpp
    piece.cpp
)

ki18n_wrap_ui(klickety_SRCS bgselector.ui customgame.ui gameconfig.ui)

kconfig_add_kcfg_files(klickety_SRCS settings.kcfgc)

kde4_add_app_icon(klickety_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-klickety.png")

add_executable(klickety ${klickety_SRCS})

target_link_libraries(klickety  KF5::NotifyConfig KF5KDEGames KF5::KIOCore KF5::KDELibs4Support)

install(TARGETS klickety ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES klickety.kcfg klickety.notifyrc DESTINATION ${DATA_INSTALL_DIR}/klickety)
install(FILES klicketyui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/klickety)

install(PROGRAMS org.kde.klickety.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(PROGRAMS ksame.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

if(NOT WIN32)
    install(PROGRAMS klickety-2.0-inherit-ksame-highscore.pl DESTINATION ${KCONF_UPDATE_INSTALL_DIR})
    install(FILES klickety.upd DESTINATION ${KCONF_UPDATE_INSTALL_DIR})
endif(NOT WIN32)

add_subdirectory(pics)
add_subdirectory(themes)
add_subdirectory(doc)

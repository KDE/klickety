project(klickety)


cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
set (QT_MIN_VERSION "5.3.0")

find_package(ECM 1.7.0 REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets )
find_package(KF5 REQUIRED COMPONENTS
    CoreAddons
    Config
    WidgetsAddons
    Config
    I18n
    ConfigWidgets
    XmlGui
    KIO
    DocTools
    NotifyConfig
    DBusAddons
    Notifications)

find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMAddAppIcon)
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

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/*-apps-klickety.png")
ecm_add_app_icon(klickety_SRCS ICONS ${ICONS_SRCS})

add_executable(klickety ${klickety_SRCS})

target_link_libraries(klickety
    KF5::NotifyConfig
    KF5::Notifications
    KF5KDEGames
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::KIOWidgets
    KF5::KIONTLM
    KF5::XmlGui
    KF5::DBusAddons)

install(TARGETS klickety ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES klickety.notifyrc DESTINATION ${KDE_INSTALL_KNOTIFY5RCDIR} )
install(FILES klickety.kcfg DESTINATION ${KDE_INSTALL_DATADIR}/klickety)
install(FILES klicketyui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/klickety)

install(PROGRAMS org.kde.klickety.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(PROGRAMS org.kde.ksame.desktop DESTINATION ${KDE_INSTALL_APPDIR})

if(NOT WIN32)
    install(PROGRAMS klickety-2.0-inherit-ksame-highscore.pl DESTINATION ${KDE_INSTALL_KCONFUPDATEDIR})
    install(FILES klickety.upd DESTINATION ${KDE_INSTALL_KCONFUPDATEDIR})
endif(NOT WIN32)

add_subdirectory(pics)
add_subdirectory(themes)
add_subdirectory(doc)
feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

set (RELEASE_SERVICE_VERSION_MAJOR "20")
set (RELEASE_SERVICE_VERSION_MINOR "12")
set (RELEASE_SERVICE_VERSION_MICRO "0")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")
set (RELEASE_SERVICE_COMPACT_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}${RELEASE_SERVICE_VERSION_MINOR}${RELEASE_SERVICE_VERSION_MICRO}")

project(klickety VERSION "2.1.${RELEASE_SERVICE_COMPACT_VERSION}")

set (QT_MIN_VERSION "5.10.0")
set (KF5_MIN_VERSION "5.46.0")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED CONFIG)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Widgets )
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    Config
    Crash
    WidgetsAddons
    I18n
    ConfigWidgets
    XmlGui
    KIO
    DocTools
    DBusAddons
)

find_package(KF5KDEGames 4.9.0 REQUIRED)

include(FeatureSummary)
include(ECMAddAppIcon)
include(ECMInstallIcons)
include(ECMSetupVersion)
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)

add_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_USE_QSTRINGBUILDER
    -DQT_STRICT_ITERATORS
    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    -DQT_DISABLE_DEPRECATED_BEFORE=0x050A00
    -DQT_DEPRECATED_WARNINGS_SINCE=0x060000
    -DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x052E00
    -DKF_DEPRECATED_WARNINGS_SINCE=0x060000
    -DQT_NO_KEYWORDS
)
if (${KF5Config_VERSION} STRGREATER "5.56.0")
        add_definitions(-DQT_NO_FOREACH)
        MESSAGE(STATUS "compile without foreach")
endif()


set(klickety_SRCS
    gamescene.cpp
    gameview.cpp
    main.cpp
    mainwindow.cpp
    piece.cpp
)

ecm_setup_version(${PROJECT_VERSION} VARIABLE_PREFIX KLICKETY VERSION_HEADER klickety_version.h)

ki18n_wrap_ui(klickety_SRCS bgselectorconfig.ui customgameconfig.ui gameconfig.ui)

kconfig_add_kcfg_files(klickety_SRCS settings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/*-apps-klickety.png")
ecm_add_app_icon(klickety_SRCS ICONS ${ICONS_SRCS})

add_executable(klickety ${klickety_SRCS})

target_link_libraries(klickety
    KF5::Crash
    KF5::DBusAddons
    KF5::KIOCore
    KF5::KIOFileWidgets
    KF5::KIONTLM
    KF5::KIOWidgets
    KF5::XmlGui
    KF5KDEGames
)

install(TARGETS klickety ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES klickety.kcfg DESTINATION ${KDE_INSTALL_DATADIR}/klickety)
install(FILES klicketyui.rc DESTINATION ${KDE_INSTALL_KXMLGUI5DIR}/klickety)

install(PROGRAMS org.kde.klickety.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(PROGRAMS org.kde.ksame.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.klickety.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.ksame.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

if(NOT WIN32)
    install(PROGRAMS klickety-2.0-inherit-ksame-highscore.pl DESTINATION ${KDE_INSTALL_KCONFUPDATEDIR})
    install(FILES klickety.upd DESTINATION ${KDE_INSTALL_KCONFUPDATEDIR})
endif(NOT WIN32)

add_subdirectory(sounds)
add_subdirectory(pics)
add_subdirectory(themes)
add_subdirectory(doc)

ki18n_install(po)
kdoctools_install(po)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

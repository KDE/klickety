ecm_setup_version(${klickety_VERSION}
    VARIABLE_PREFIX KLICKETY
    VERSION_HEADER klickety_version.h
)

add_executable(klickety)

target_sources(klickety PRIVATE
    gamescene.cpp gamescene.h
    gameview.cpp gameview.h
    main.cpp
    mainwindow.cpp mainwindow.h
    piece.cpp piece.h

    klickety.qrc
)

if(APPLE)
    set_target_properties(klickety PROPERTIES
           MACOSX_BUNDLE_DISPLAY_NAME "Klickety"
        MACOSX_BUNDLE_BUNDLE_NAME "Klickety"
        MACOSX_BUNDLE_LONG_VERSION_STRING "Klickety ${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${RELEASE_SERVICE_VERSION}"
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.klickety"
        MACOSX_BUNDLE_COPYRIGHT "2024 The Klickety Developers")
endif()


ki18n_wrap_ui(klickety
    bgselectorconfig.ui
    customgameconfig.ui
    gameconfig.ui
)

kconfig_add_kcfg_files(klickety settings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/pics/*-apps-klickety.png")
ecm_add_app_icon(klickety ICONS ${ICONS_SRCS})

target_link_libraries(klickety
    KDEGames6
    KF6::Crash
    KF6::DBusAddons
    KF6::I18n
    KF6::KIOWidgets
    KF6::XmlGui
    KF6::IconThemes
)

install(TARGETS klickety ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES klickety.kcfg DESTINATION ${KDE_INSTALL_DATADIR}/klickety)

install(PROGRAMS org.kde.klickety.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(PROGRAMS org.kde.ksame.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.klickety.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.ksame.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

if(NOT WIN32)
    install(PROGRAMS klickety-2.0-inherit-ksame-highscore.pl DESTINATION ${KDE_INSTALL_KCONFUPDATEDIR})
    install(FILES klickety.upd DESTINATION ${KDE_INSTALL_KCONFUPDATEDIR})
endif(NOT WIN32)

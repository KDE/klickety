ecm_setup_version(${klickety_VERSION}
    VARIABLE_PREFIX KLICKETY
    VERSION_HEADER klickety_version.h
)

add_executable(klickety)

target_sources(klickety PRIVATE
    gamescene.cpp
    gameview.cpp
    main.cpp
    mainwindow.cpp
    piece.cpp

    klickety.qrc
)

ki18n_wrap_ui(klickety
    bgselectorconfig.ui
    customgameconfig.ui
    gameconfig.ui
)

kconfig_add_kcfg_files(klickety settings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/pics/*-apps-klickety.png")
ecm_add_app_icon(klickety ICONS ${ICONS_SRCS})

target_link_libraries(klickety
    KF5KDEGames
    KF${KF_MAJOR_VERSION}::Crash
    KF${KF_MAJOR_VERSION}::DBusAddons
    KF${KF_MAJOR_VERSION}::KIOWidgets
    KF${KF_MAJOR_VERSION}::XmlGui
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

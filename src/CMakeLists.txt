set(klickety_SRCS
    gamescene.cpp
    gameview.cpp
    main.cpp
    mainwindow.cpp
    piece.cpp
)

ecm_setup_version(${klickety_VERSION}
    VARIABLE_PREFIX KLICKETY
    VERSION_HEADER klickety_version.h
)

ki18n_wrap_ui(klickety_SRCS
    bgselectorconfig.ui
    customgameconfig.ui
    gameconfig.ui
)

kconfig_add_kcfg_files(klickety_SRCS settings.kcfgc)

file(GLOB ICONS_SRCS "${CMAKE_SOURCE_DIR}/pics/*-apps-klickety.png")
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

project(klickety)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(klickety_SRCS
    gamescene.cpp
    gameview.cpp
    main.cpp
    mainwindow.cpp
    piece.cpp
)

kde4_add_ui_files(klickety_SRCS bgselector.ui customgame.ui gameconfig.ui)

kde4_add_kcfg_files(klickety_SRCS settings.kcfgc)

kde4_add_app_icon(klickety_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/pics/hi*-app-klickety.png")

kde4_add_executable(klickety ${klickety_SRCS})

target_link_libraries(klickety ${KDE4_KDEUI_LIBS} ${KDE4_KNOTIFYCONFIG_LIBS} kdegames ${KDE4_KIO_LIBRARY})

install(TARGETS klickety ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES klickety.kcfg klickety.notifyrc klicketyui.rc DESTINATION ${DATA_INSTALL_DIR}/klickety)
install(PROGRAMS klickety.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(PROGRAMS ksame.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

add_subdirectory(pics)
add_subdirectory(themes)

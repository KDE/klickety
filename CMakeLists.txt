project(klickety)

if(NOT COMPILING_KDEGAMES)
	find_package(KDE4 REQUIRED)
	include(KDE4Defaults)
	include(MacroLibrary)
	find_package(KDEGames REQUIRED)

	add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
	add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
	include_directories(${QDBUS_INCLUDE_DIRS} ${KDE4_INCLUDES} ${KDEGAMES_INCLUDE_DIRS})
endif(NOT COMPILING_KDEGAMES)

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

if(NOT WIN32)
    install(PROGRAMS klickety-2.0-inherit-ksame-highscore.pl DESTINATION ${KCONF_UPDATE_INSTALL_DIR})
    install(FILES klickety.upd DESTINATION ${KCONF_UPDATE_INSTALL_DIR})
endif(NOT WIN32)

add_subdirectory(pics)
add_subdirectory(themes)

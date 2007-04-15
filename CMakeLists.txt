project(klickety)
add_subdirectory( pics ) 

include_directories( ${CMAKE_SOURCE_DIR}/libksirtet ${CMAKE_SOURCE_DIR}/libkdegames/highscore ${CMAKE_SOURCE_DIR}/libkdegames )


if(KDE4_ENABLE_FPIE)
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_CXX_FPIE_FLAGS}")
endif(KDE4_ENABLE_FPIE)

########### next target ###############

set(klickety_SRCS 
   piece.cpp 
   klboard.cpp 
   field.cpp 
   highscores.cpp 
   main.cpp 
   klfactory.cpp
   )

kde4_automoc(${klickety_SRCS})

kde4_add_executable(klickety ${klickety_SRCS})

if(KDE4_ENABLE_FPIE)
    target_link_libraries(klickety  ${KDE4_KDECORE_LIBS} ksirtetlib ${KDE4_PIE_LDFLAGS})
else(KDE4_ENABLE_FPIE)
    target_link_libraries(klickety  ${KDE4_KDECORE_LIBS} ksirtetlib )
endif(KDE4_ENABLE_FPIE)

install(TARGETS klickety  DESTINATION ${BIN_INSTALL_DIR} )


########### install files ###############

install( FILES klickety.desktop  DESTINATION  ${XDG_APPS_DIR} )
install( FILES klickety.notifyrc  DESTINATION  ${DATA_INSTALL_DIR}/klickety )
install( FILES klicketyui.rc  DESTINATION  ${DATA_INSTALL_DIR}/klickety )




#original Makefile.am contents follow:

#SUBDIRS = pics
#
#INCLUDES = -I$(top_builddir)/libksirtet -I$(top_srcdir)/libksirtet -I$(top_srcdir)/libkdegames/highscore -I$(top_srcdir)/libkdegames $(all_includes)
#
#KDE_CXXFLAGS = $(KDE_USE_FPIE)
#
#bin_PROGRAMS     = klickety
#klickety_LDADD   = $(top_builddir)/libksirtet/base/libksirtetbase.la
#klickety_DEPENDENCIES = $(LIB_KDEGAMES_DEP)
#klickety_LDFLAGS = $(KDE_USE_PIE) $(LIB_KDEGAMES) $(all_libraries) $(KDE_RPATH)
#klickety_SOURCES = piece.cpp board.cpp field.cpp highscores.cpp main.cpp
#METASOURCES      = board.moc field.moc main.moc
#
#rcdir   = $(kde_datadir)/klickety
#rc_DATA = klicketyui.rc
#
#xdg_apps_DATA = klickety.desktop
#
#appdatadir = $(kde_datadir)/klickety
#appdata_DATA = eventsrc
#
#messages: rc.cpp
#	$(XGETTEXT) rc.cpp $(klickety_SOURCES) -o $(podir)/klickety.pot
#
## for system-wide highscore file
#DESTBIN = $(DESTDIR)$(bindir)/$(bin_PROGRAMS)
#DESTHIGHSCORES = $(DESTDIR)$(HIGHSCORE_DIRECTORY)
#DESTSCORES = $(DESTDIR)$(HIGHSCORE_DIRECTORY)/$(bin_PROGRAMS).scores
#
#install-data-local:
#	@(test x$(HIGHSCORE_DIRECTORY) != x \
#	&& echo "********************************************************" \
#	&& echo "" \
#	&& echo "This game is installed sgid \"games\" to use the" \
#	&& echo "system-wide highscore file (in "$(HIGHSCORE_DIRECTORY)")." \
#	&& echo "" \
#	&& echo "If the system-wide highscore file does not exist, it is" \
#        && echo "created with the correct ownership and permissions. See the" \
#        && echo "INSTALL file in \"kdegames/libkdegames/highscore\" for details." \
#	&& echo "" \
#	&& echo "********************************************************" \
#	) || true
#
#install-exec-hook:
#	@(test x$(HIGHSCORE_DIRECTORY) != x \
#        && ((chown $(highscore_user):$(highscore_group) $(DESTBIN)) \
#	|| echo "Error: Could not install the game with correct permissions !!" \
#	)) || true
#
#	@(test x$(HIGHSCORE_DIRECTORY) != x \
#		&& ((mkdir -p $(DESTHIGHSCORES) && chown $(highscore_user):$(highscore_group) $(DESTHIGHSCORES) \
#		&& chmod 750 $(DESTHIGHSCORES)) \
#	|| echo "Error: Could not create the highscore directory with correct permissions !!" \
#	)) || true
#
#	@(test x$(HIGHSCORE_DIRECTORY) != x \
#        && ((chown $(highscore_user):$(highscore_group) $(DESTBIN)) \
#	|| echo "Error: Could not install the game with correct permissions !!" \
#	)) || true
#
#	@(test ${setgid} = true \
#        && ((chmod 2755  $(DESTBIN)) \
#	|| echo "Error: Could not install the game with correct permissions !!" \
#	)) || true
#
#	@(test x$(HIGHSCORE_DIRECTORY) != x \
#        && ((touch $(DESTSCORES) && chown $(highscore_user):$(highscore_group) $(DESTSCORES) \
#	&& chmod 0660 $(DESTSCORES)) \
#	|| echo "Error: Could not create system-wide highscore file with correct permissions !!" \
#	)) || true
INCLUDES = -I$(top_srcdir)/ksirtet -I$(top_srcdir)/libkdegames -I$(top_srcdir)/kmines $(all_includes)

bin_PROGRAMS     = klickety
klickety_LDADD   = $(top_builddir)/ksirtet/base/libksirtetbase.la
klickety_LDFLAGS = $(all_libraries) $(KDE_RPATH)
klickety_SOURCES = piece.cpp board.cpp field.cpp highscores.cpp main.cpp
METASOURCES      = board.moc field.moc main.moc

rcdir   = $(kde_datadir)/klickety
rc_DATA = klicketyui.rc klicketyconfig.rc

KDE_ICON = klickety
appsdir = $(kde_appsdir)/Games/Arcade
apps_DATA = klickety.desktop
EXTRA_DIST = $(apps_DATA)

messages: rc.cpp
	$(XGETTEXT) rc.cpp $(klickety_SOURCES) -o $(podir)/klickety.pot

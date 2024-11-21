/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"
#include "settings.h"
#include "klickety_version.h"

#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>
#define HAVE_KICONTHEME __has_include(<KIconTheme>)
#if HAVE_KICONTHEME
#include <KIconTheme>
#endif

#define HAVE_STYLE_MANAGER __has_include(<KStyleManager>)
#if HAVE_STYLE_MANAGER
#include <KStyleManager>
#endif
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main( int argc, char* argv[] )
{
#if HAVE_KICONTHEME
    KIconTheme::initTheme();
#endif
    QApplication app( argc, argv );
#if HAVE_STYLE_MANAGER
    KStyleManager::initStyle();
#else // !HAVE_STYLE_MANAGER
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    QApplication::setStyle(QStringLiteral("breeze"));
#endif // defined(Q_OS_MACOS) || defined(Q_OS_WIN)
#endif // HAVE_STYLE_MANAGER
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral( "klickety" )));

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("klickety"));

    KAboutData aboutData( QStringLiteral( "klickety" ), i18n( "Klickety" ), QStringLiteral( KLICKETY_VERSION_STRING ),
                          i18n( "Klickety is an adaptation of the \"clickomania\" game" ),
                          KAboutLicense::GPL,
                          i18n( "(c) 2002-2005, Nicolas Hadacek\n(c) 2010, Ni Hui" ),
                          QString(),
                          QStringLiteral("https://apps.kde.org/klickety"));

    KAboutData::setApplicationData( aboutData );

    KCrash::initialize();

    QCommandLineParser parser;
    parser.addOption( QCommandLineOption({QStringLiteral("KSameMode")}, i18n("Start with KSame compatibility mode")));

    aboutData.setupCommandLine( &parser );
    parser.process( app );
    aboutData.processCommandLine( &parser );

    bool kSameMode = parser.isSet( QStringLiteral("KSameMode") );

    // we use different file for storing ksame mode configuration
    if ( kSameMode ) {
        Settings::instance( QStringLiteral( "ksamerc" ) );
        aboutData.setShortDescription( i18n( "A little game about balls and how to get rid of them" ) );
        aboutData.addAuthor( i18n( "Marcus Kreutzberger"), i18n( "Original author" ), QStringLiteral("kreutzbe@informatik.mu-luebeck.de") );
        aboutData.addAuthor( i18n( "Henrique Pinto"), i18n( "Past maintainer" ), QStringLiteral("henrique.pinto@kdemail.net") );
        aboutData.addAuthor( i18n( "Ni Hui" ), i18n( "Integration with Klickety. Current maintainer" ), QStringLiteral("shuizhuyuanluo@126.com") );
        aboutData.addCredit( i18n( "Johann Ollivier Lapeyre"), i18n("Artwork"), QStringLiteral("johann.ollivierlapeyre@gmail.com") );
    }
    else {
        Settings::instance( QStringLiteral( "klicketyrc" ) );
        aboutData.addAuthor( i18n( "Nicolas Hadacek" ), i18n( "Original author" ), QStringLiteral("hadacek@kde.org") );
        aboutData.addAuthor( i18n( "Ni Hui" ), i18n( "Rewrite for KDE4. Current maintainer" ), QStringLiteral("shuizhuyuanluo@126.com") );
        aboutData.addCredit( i18n( "Dan Hill" ), i18n( "Icons" ) );
    }

    // see if we are starting with session management
    if ( app.isSessionRestored() ) {
        kRestoreMainWindows<MainWindow>();
    }
    else {
        MainWindow* window = new MainWindow( kSameMode );
        window->show();
    }

    KDBusService service;

    return app.exec();
}

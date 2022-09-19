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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <Kdelibs4ConfigMigrator>
#endif
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main( int argc, char* argv[] )
{
    // Fixes blurry icons with fractional scaling
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    QApplication app( argc, argv );
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral( "klickety" )));

    KLocalizedString::setApplicationDomain("klickety");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // Migrate pre-existing (4.x) configuration
    Kdelibs4ConfigMigrator migrate(QStringLiteral("klickety"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("klicketyrc"));
    migrate.setUiFiles(QStringList() << QStringLiteral("klicketyui.rc"));
    migrate.migrate();
#endif
    KAboutData aboutData( QStringLiteral( "klickety" ), i18n( "Klickety" ), QStringLiteral( KLICKETY_VERSION_STRING ),
                          i18n( "Klickety is an adaptation of the \"clickomania\" game" ),
                          KAboutLicense::GPL,
                          i18n( "(c) 2002-2005, Nicolas Hadacek\n(c) 2010, Ni Hui" ),
                          QString(),
                          QStringLiteral("https://apps.kde.org/klickety"));

    QCommandLineParser parser;
    KAboutData::setApplicationData( aboutData );
    KCrash::initialize();
    parser.addOption( QCommandLineOption( QStringList() <<  QStringLiteral( "KSameMode" ), i18n( "Start with KSame compatibility mode" ) ) );

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

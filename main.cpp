/*
 * Copyright (c) 2010 Ni Hui <shuizhuyuanluo@126.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "mainwindow.h"
#include "settings.h"

#include <KAboutData>
#include <KDBusService>
#include <Kdelibs4ConfigMigrator>
#include <KLocalizedString>

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

#include <ctime>

int main( int argc, char* argv[] )
{
    QApplication app( argc, argv );
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral( "klickety" )));

    // Migrate pre-existing (4.x) configuration
    QStringList configFiles;
    configFiles.append(QStringLiteral("klicketyrc"));
    configFiles.append(QStringLiteral("klickety.notifyrc"));

    Kdelibs4ConfigMigrator migrate(QStringLiteral("klickety"));
    migrate.setConfigFiles(configFiles);
    migrate.setUiFiles(QStringList() << QStringLiteral("klicketyui.rc"));
    migrate.migrate();

    qsrand( std::time( nullptr ) );
    KAboutData aboutData( QStringLiteral( "klickety" ), i18n( "Klickety" ), QStringLiteral( "2.0" ),
                          i18n( "Klickety is an adaptation of the \"clickomania\" game" ),
                          KAboutLicense::GPL,
                          i18n( "(c) 2002-2005, Nicolas Hadacek\n(c) 2010, Ni Hui" ) );
    aboutData.setHomepage(QStringLiteral( "http://games.kde.org/klickety" ) );

    QCommandLineParser parser;
    KAboutData::setApplicationData( aboutData );
    parser.addVersionOption();
    parser.addHelpOption();
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
        RESTORE(MainWindow( kSameMode ));
    }
    else {
        MainWindow* window = new MainWindow( kSameMode );
        window->show();
    }

    KDBusService service;

    return app.exec();
}

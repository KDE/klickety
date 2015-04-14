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


#include <KLocalizedString>

#include <ctime>
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main( int argc, char* argv[] )
{
    QApplication app(argc, argv);

    qsrand( time(0) );
    KAboutData aboutData( "klickety", i18n( "Klickety" ), "2.0",
                          i18n( "Klickety is an adaptation of the \"clickomania\" game" ),
                          KAboutLicense::GPL,
                          i18n( "(c) 2002-2005, Nicolas Hadacek\n(c) 2010, Ni Hui" ),
                          "http://games.kde.org/klickety" );

    QCommandLineParser parser;
    KAboutData::setApplicationData(aboutData);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addOption(QCommandLineOption(QStringList() <<  QLatin1String("KSameMode"), i18n( "Start with KSame compatibility mode" )));

    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    bool KSameMode = parser.isSet( "KSameMode" );

    // set kconfig instance
    // we use different file for storing ksame mode configuration
    if ( KSameMode )
        Settings::instance( QLatin1String( "ksamerc" ) );
    else {
        Settings::instance( QLatin1String( "klicketyrc" ) );
    }
    

    if ( KSameMode ) {
        aboutData.setProgramIconName( "ksame" );
        aboutData.setShortDescription( i18n( "A little game about balls and how to get rid of them" ) );
        aboutData.addAuthor( i18n( "Marcus Kreutzberger"), i18n( "Original author" ), "kreutzbe@informatik.mu-luebeck.de" );
        aboutData.addAuthor( i18n( "Henrique Pinto"), i18n( "Past maintainer" ), "henrique.pinto@kdemail.net" );
        aboutData.addAuthor( i18n( "Ni Hui" ), i18n( "Integration with Klickety. Current maintainer" ), "shuizhuyuanluo@126.com" );
        aboutData.addCredit( i18n( "Johann Ollivier Lapeyre"), i18n("Artwork"), "johann.ollivierlapeyre@gmail.com" );
    }
    else {
        aboutData.addAuthor( i18n( "Nicolas Hadacek" ), i18n( "Original author" ), "hadacek@kde.org" );
        aboutData.addAuthor( i18n( "Ni Hui" ), i18n( "Rewrite for KDE4. Current maintainer" ), "shuizhuyuanluo@126.com" );
        aboutData.addCredit( i18n( "Dan Hill" ), i18n( "Icons" ) );
    }



    // see if we are starting with session management
    if ( app.isSessionRestored() ) {
        RESTORE(MainWindow( KSameMode ));
    }
    else {
        MainWindow* window = new MainWindow( KSameMode );
        window->show();
    }

    return app.exec();
}

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

#include <ctime>
#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>
#include <KLocale>

#include "mainwindow.h"
#include "settings.h"

int main( int argc, char* argv[] )
{
    qsrand( time(0) );
    KAboutData aboutData( "klickety", 0, ki18n( "Klickety" ), "2.0",
                          ki18n( "Klickety is an adaptation of the \"clickomania\" game" ),
                          KAboutData::License_GPL,
                          ki18n( "(c) 2002-2005, Nicolas Hadacek\n(c) 2010, Ni Hui" ),
                          KLocalizedString(), "http://games.kde.org/klickety" );
    aboutData.addAuthor( ki18n( "Nicolas Hadacek" ), ki18n( "Original author" ), "hadacek@kde.org" );
    aboutData.addAuthor( ki18n( "Ni Hui" ), ki18n( "Rewrite for KDE4. Current maintainer" ), "shuizhuyuanluo@126.com" );
    aboutData.addCredit( ki18n( "Dan Hill" ), ki18n( "Icons" ) );

    KCmdLineArgs::init( argc, argv, &aboutData );

    KCmdLineOptions options;
    options.add( "VeryEasy", ki18n( "Start with Very Easy difficulty level" ) );
    options.add( "Easy", ki18n( "Start with Easy difficulty level" ) );
    options.add( "Medium", ki18n( "Start with Medium difficulty level" ) );
    options.add( "Hard", ki18n( "Start with Hard difficulty level" ) );

    options.add( "KSameMode", ki18n( "Start with KSame compability mode" ) );
    KCmdLineArgs::addCmdLineOptions( options );

    KApplication app;
    KGlobal::locale()->insertCatalog( "libkdegames" );
    //resource directory for KNewStuff2
//     KStandardDirs::locateLocal("appdata", "themes/");

    bool KSameMode = false;
    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    if ( args->isSet( "VeryEasy" ) )
        Settings::setLevel( KGameDifficulty::VeryEasy );
    if ( args->isSet( "Easy" ) )
        Settings::setLevel( KGameDifficulty::Easy );
    if ( args->isSet( "Medium" ) )
        Settings::setLevel( KGameDifficulty::Medium );
    if ( args->isSet( "Hard" ) )
        Settings::setLevel( KGameDifficulty::Hard );
    if ( args->isSet( "KSameMode" ) )
        KSameMode = true;
    args->clear();

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

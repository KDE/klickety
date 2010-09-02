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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KGameDifficulty>
#include <KXmlGuiWindow>

class KAction;
class KGameClock;
class GameScene;
class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
    public:
        explicit MainWindow( QWidget* parent = 0 );
        ~MainWindow();
    private Q_SLOTS:
        void configureNotifications();
        void configureSettings();
        void customLevelChanged( int key );
        void levelChanged( KGameDifficulty::standardLevel level );
        void loadSettings();
        void newGame();
        void pauseGame( bool isPaused );
        void restartGame();
        void loadGame();
        void saveGame();
        void changeRemainCount( int remainCount );
        void changeTime( const QString& newTime );
        void showHighscores();
        void onGameOver( int remainCount );
    private:
        bool confirmAbort();
        void setupActions();
        GameScene* m_scene;
        KGameClock* m_gameClock;
        KAction* m_pauseAction;
        KAction* m_randomBoardAction;
};

#endif // MAINWINDOW_H

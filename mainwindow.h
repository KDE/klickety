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

#include <KXmlGuiWindow>

#include <QLabel>

class KToggleAction;
class KGameClock;
class GameScene;
class MainWindow : public KXmlGuiWindow
{
    Q_OBJECT
    public:
        explicit MainWindow( bool kSameMode = false, QWidget* parent = nullptr );
        ~MainWindow();
    private Q_SLOTS:
        void configureSettings();
        void loadSettings();
        void newGame( int gameId );
        void newNumGame();
        void pauseGame( bool isPaused );
        void restartGame();
        void loadGame();
        void saveGame();
        void changeMarkedCount( int markedCount );
        void changeScore( int remainCount );
        void changeRemainCount( int remainCount );
        void changeTime( const QString& newTime );
        void showHighscores();
        void onGameOver( int remainCount );
        void setSoundsEnabled( bool enabled );
    private:
        bool confirmAbort();
        void setupActions();
        void setupDifficulties();
        bool m_kSameMode;
        GameScene* m_scene;
        /** Klickety mode only */
        KGameClock* m_gameClock;
        /** KSame mode only */
        int m_gameScore, m_lastRemainCount;
        KToggleAction* m_pauseAction;
        QLabel* m_statusBarLabelLeft;
        QLabel* m_statusBarLabelRight;
};

#endif // MAINWINDOW_H

/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>

#include <QLabel>
#include <cstdlib>

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
        void newGame();
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
        void newGameWithId(int gameId);
        bool confirmAbort();
        void setupActions();
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

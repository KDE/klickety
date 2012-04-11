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

#include "bgselector.h"
#include "customgame.h"
#include "gameconfig.h"
#include "gamescene.h"
#include "gameview.h"
#include "settings.h"

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KConfigDialog>
#include <KFileDialog>
#include <KGameClock>
#include <KgDifficulty>
#include <KgThemeSelector>
#include <KInputDialog>
#include <KLocale>
#include <KMessageBox>
#include <KNotifyConfigWidget>
#include <KScoreDialog>
#include <KStandardAction>
#include <KStandardGameAction>
#include <KStatusBar>
#include <KToggleAction>

#include <QPointer>

MainWindow::MainWindow( bool KSameMode, QWidget* parent )
: KXmlGuiWindow(parent),
m_KSameMode(KSameMode),
m_gameClock(NULL),
m_gameScore(0),
m_lastRemainCount(0)
{
    m_scene = new GameScene;
    GameView* view = new GameView( m_scene );
    view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setFrameStyle( QFrame::NoFrame );
    view->setCacheMode( QGraphicsView::CacheBackground );
    setCentralWidget( view );

    if ( m_KSameMode ) {
//         statusBar()->insertItem( i18n( "Colors: XX" ), 1 );
//         statusBar()->insertItem( i18n( "Board: XXXXXX" ), 2 );
        statusBar()->insertItem( i18n( "Marked: 0" ), 3 );
        statusBar()->insertItem( i18n( "Score: 0" ), 4 );
        connect( m_scene, SIGNAL(remainCountChanged(int)), this, SLOT(changeScore(int)) );
        connect( m_scene, SIGNAL(markedCountChanged(int)), this, SLOT(changeMarkedCount(int)) );
    }
    else {
        m_gameClock = new KGameClock( this, KGameClock::MinSecOnly );
        statusBar()->insertItem( i18n( "Pieces: 0" ), 0 );
        statusBar()->insertItem( i18n( "Time: 00:00" ), 1 );
        connect( m_scene, SIGNAL(remainCountChanged(int)), this, SLOT(changeRemainCount(int)) );
        connect( m_gameClock, SIGNAL(timeChanged(QString)), this, SLOT(changeTime(QString)) );
    }

    connect( m_scene, SIGNAL(gameFinished(int)), this, SLOT(onGameOver(int)) );

    setupActions();

    loadSettings();

    newGame( qrand() );
}

MainWindow::~MainWindow()
{
}

void MainWindow::configureNotifications()
{
    KNotifyConfigWidget::configure( this );
}

void MainWindow::configureSettings()
{
    if ( KConfigDialog::showDialog( QLatin1String( "settings" ) ) )
        return;

    KConfigDialog* dialog = new KConfigDialog( this, QLatin1String( "settings" ), Settings::self() );
    dialog->addPage( new GameConfig( dialog ), i18n( "General" ), QLatin1String( "games-config-options" ) );
    dialog->addPage( new KgThemeSelector( m_scene->themeProvider() ), i18n( "Theme" ), QLatin1String( "games-config-theme" ) );
    dialog->addPage( new BackgroundSelector( dialog ), i18n( "Background" ), QLatin1String( "games-config-background" ) );
    if ( !m_KSameMode )
        dialog->addPage( new CustomGameConfig( dialog ), i18n( "Custom Game" ), QLatin1String( "games-config-custom" ) );
    connect(m_scene->themeProvider(), SIGNAL(currentThemeChanged(const KgTheme*)), SLOT(loadSettings())); //setBackgroundType!
    connect( dialog, SIGNAL(settingsChanged(QString)), this, SLOT(loadSettings()) );
    dialog->setHelp( QString(), QLatin1String( "klickety" ) );
    dialog->show();
}

void MainWindow::loadSettings()
{
    m_scene->setShowBoundLines( Settings::showBoundLines() );
    m_scene->setEnableAnimation( Settings::enableAnimation() );
    m_scene->setEnableHighlight( Settings::enableHighlight() );
    m_scene->setBackgroundType( Settings::bgType() );
}

void MainWindow::newGame( int gameId )
{
    if ( !confirmAbort() )
        return;

    m_pauseAction->setChecked( false );
    m_pauseAction->setEnabled( true );

    if ( m_KSameMode ) {
        m_gameScore = 0;
        m_lastRemainCount = 15 * 10;
        m_scene->startNewGame( 15, 10, 3, gameId );
        return;
    }

    m_gameClock->restart();

    switch ( Kg::difficultyLevel() ) {
        case KgDifficultyLevel::VeryEasy:
            m_scene->startNewGame( 10, 16, 3, gameId );
            break;
        case KgDifficultyLevel::Easy:
            m_scene->startNewGame( 10, 16, 4, gameId );
            break;
        case KgDifficultyLevel::Medium:
            m_scene->startNewGame( 10, 16, 5, gameId );
            break;
        case KgDifficultyLevel::Hard:
            m_scene->startNewGame( 10, 16, 6, gameId );
            break;
        case KgDifficultyLevel::Custom:
            m_scene->startNewGame( Settings::customWidth(),
                                   Settings::customHeight(),
                                   Settings::customColorCount(), gameId );
            break;
        default:
            break;
    }
}

void MainWindow::newNumGame()
{
    if ( !confirmAbort() )
        return;

    bool ok = false;
    int userGameId = KInputDialog::getInteger( i18n( "Select Board" ),
                                                i18n( "Select a board number:" ),
                                                qrand(), 1, RAND_MAX, 1,
                                                &ok, this );
    if ( ok )
        newGame( userGameId );
}

void MainWindow::pauseGame( bool isPaused )
{
    m_scene->setPaused( isPaused );
    if ( !m_KSameMode ) {
        if ( isPaused )
            m_gameClock->pause();
        else
            m_gameClock->resume();
    }
}

void MainWindow::restartGame()
{
    if ( confirmAbort() ) {
        m_pauseAction->setChecked( false );
        m_pauseAction->setEnabled( true );
        if ( m_KSameMode ) {
            m_gameScore = 0;
            m_lastRemainCount = 0;
        }
        else {
            m_gameClock->restart();
        }
        m_scene->restartGame();
    }
}

void MainWindow::loadGame()
{
    QString fileName = KFileDialog::getOpenFileName( KUrl(), QLatin1String( "*.klickety" ), this );
    if ( fileName.isEmpty() || !confirmAbort() )
        return;

    m_pauseAction->setChecked( false );
    m_pauseAction->setEnabled( true );
    KConfig config( fileName, KConfig::SimpleConfig );
    KConfigGroup group = config.group( "Savegame" );
    m_scene->loadGame( group );
}

void MainWindow::saveGame()
{
    QString fileName = KFileDialog::getSaveFileName( KUrl(), QLatin1String( "*.klickety" ), this );
    if ( fileName.isEmpty() )
        return;
    KConfig config( fileName, KConfig::SimpleConfig );
    KConfigGroup group = config.group( "Savegame" );
    m_scene->saveGame( group );
}

void MainWindow::changeMarkedCount( int markedCount )
{
    int markedScore = ( markedCount < 2 ) ? 0 : ( ( markedCount - 2 ) * ( markedCount - 2 ) );
    statusBar()->changeItem( i18np( "Marked: %2 (1 Point)", "Marked: %2 (%1 Points)", markedScore, markedCount ), 3 );
}

void MainWindow::changeScore( int remainCount )
{
    if ( m_lastRemainCount == 0 ) {
        // new game or restart
        m_lastRemainCount = remainCount;
        statusBar()->changeItem( i18n( "Score: 0" ), 4 );
        return;
    }
    int removedCount = m_lastRemainCount - remainCount;
    if ( removedCount > 0 ) {
        // normal move
        int score = ( removedCount < 2 ) ? 0 : ( ( removedCount - 2 ) * ( removedCount - 2 ) );
        m_gameScore += score;
    }
    else {
        // undo action, minus the score
        int score = ( removedCount > -2 ) ? 0 : ( ( removedCount + 2 ) * ( removedCount + 2 ) );
        m_gameScore -= score;
    }
    statusBar()->changeItem( i18n( "Score: %1", m_gameScore ), 4 );
    m_lastRemainCount = remainCount;
}

void MainWindow::changeRemainCount( int remainCount )
{
    statusBar()->changeItem( i18n( "Pieces: %1", remainCount ), 0 );
}

void MainWindow::changeTime( const QString& newTime )
{
    statusBar()->changeItem( i18n( "Time: %1", newTime ), 1 );
}

void MainWindow::showHighscores()
{
    if ( m_KSameMode ) {
        QPointer<KScoreDialog> d = new KScoreDialog( KScoreDialog::Name | KScoreDialog::Score, this );
        d->initFromDifficulty(Kg::difficulty(), /*setConfigGroup=*/ false);
        d->setConfigGroup( qMakePair( QByteArray( "KSame" ), i18n( "High Scores" ) ) );
        d->setHiddenConfigGroups( QList<QByteArray>() << "Very Easy" << "Easy" << "Medium" << "Hard" << "Custom" );
        d->exec();
        delete d;
        return;
    }

    QPointer<KScoreDialog> d = new KScoreDialog( KScoreDialog::Name, this );
    d->addField( KScoreDialog::Custom1, i18n( "Remaining pieces" ), QLatin1String( "remains" ) );
    d->addField( KScoreDialog::Custom2, i18n( "Time" ), QLatin1String( "time" ) );
    d->initFromDifficulty(Kg::difficulty(), /*setConfigGroup=*/ true);
    d->setHiddenConfigGroups( QList<QByteArray>() << "KSame" );
    d->hideField( KScoreDialog::Score );
    d->exec();
    delete d;
}

void MainWindow::onGameOver( int remainCount )
{
    m_pauseAction->setEnabled( false );

    if ( m_KSameMode ) {
        if ( remainCount == 0 ) {
            // if the board is empty, give a bonus
            m_gameScore += 1000;
        }

        QPointer<KScoreDialog> d = new KScoreDialog( KScoreDialog::Name | KScoreDialog::Score, this );
        d->initFromDifficulty(Kg::difficulty(), /*setConfigGroup=*/ false);
        d->setConfigGroup( qMakePair( QByteArray( "KSame" ), i18n( "High Scores" ) ) );
        d->setHiddenConfigGroups( QList<QByteArray>() << "Very Easy" << "Easy" << "Medium" << "Hard" << "Custom" );

        KScoreDialog::FieldInfo scoreInfo;
        scoreInfo[ KScoreDialog::Score ].setNum( m_gameScore );

        if ( d->addScore( scoreInfo ) )
            d->exec();
        delete d;
        return;
    }

    m_gameClock->pause();

    QPointer<KScoreDialog> d = new KScoreDialog( KScoreDialog::Name, this );
    d->addField( KScoreDialog::Custom1, i18n( "Remaining pieces" ), QLatin1String( "remains" ) );
    d->addField( KScoreDialog::Custom2, i18n( "Time" ), QLatin1String( "time" ) );
    d->initFromDifficulty(Kg::difficulty(), /*setConfigGroup=*/ true);
    d->setHiddenConfigGroups( QList<QByteArray>() << "KSame" );
    d->hideField( KScoreDialog::Score );

    KScoreDialog::FieldInfo scoreInfo;
    scoreInfo[KScoreDialog::Custom1].setNum( remainCount );
    scoreInfo[KScoreDialog::Custom2] = m_gameClock->timeString();
    // remainCount*10000000 is much bigger than a usual time seconds
    scoreInfo[KScoreDialog::Score].setNum( remainCount*10000000 + m_gameClock->seconds() );
    if ( d->addScore( scoreInfo, KScoreDialog::LessIsMore ) != 0 )
        d->exec();
    delete d;
}

bool MainWindow::confirmAbort()
{
    return m_scene->isGameFinished() || ( KMessageBox::questionYesNo( this, i18n( "Do you want to resign?" ),
        i18n( "New Game" ),KGuiItem( i18n( "Resign" ) ), KStandardGuiItem::cancel() ) == KMessageBox::Yes );
}

void MainWindow::setupActions()
{
    // game menu
    KStandardGameAction::gameNew( this, SLOT(newGame()), actionCollection() );
    if ( !m_KSameMode ) {
        KStandardGameAction::load( this, SLOT(loadGame()), actionCollection() );
        KStandardGameAction::save( this, SLOT(saveGame()), actionCollection() );
    }
    KStandardGameAction::restart( this, SLOT(restartGame()), actionCollection() );
    KStandardGameAction::highscores( this, SLOT(showHighscores()), actionCollection() );
    m_pauseAction = KStandardGameAction::pause( this, SLOT(pauseGame(bool)), actionCollection() );
    KStandardGameAction::quit( this, SLOT(close()), actionCollection() );
    KAction* m_newNumGameAction = new KAction( i18n( "New Numbered Game..." ), actionCollection() );
    actionCollection()->addAction( QLatin1String( "game_new_numeric" ), m_newNumGameAction );
    connect( m_newNumGameAction, SIGNAL(triggered(bool)), this, SLOT(newNumGame()) );

    // move menu
    KAction* undoAction = KStandardGameAction::undo( m_scene, SLOT(undoMove()), actionCollection() );
    undoAction->setEnabled( false );
    connect( m_scene, SIGNAL(canUndoChanged(bool)), undoAction, SLOT(setEnabled(bool)) );
    KAction* redoAction = KStandardGameAction::redo( m_scene, SLOT(redoMove()), actionCollection() );
    redoAction->setEnabled( false );
    connect( m_scene, SIGNAL(canRedoChanged(bool)), redoAction, SLOT(setEnabled(bool)) );

    KAction* undoAllAction = actionCollection()->addAction( QLatin1String( "move_undo_all" ) );
    undoAllAction->setIcon( KIcon( QLatin1String(  "media-skip-backward" ) ) );
    undoAllAction->setText( i18n( "Undo All" ) );
    undoAllAction->setEnabled( false );
    connect( m_scene, SIGNAL(canUndoChanged(bool)), undoAllAction, SLOT(setEnabled(bool)) );
    connect( undoAllAction, SIGNAL(triggered(bool)), m_scene, SLOT(undoAllMove()) );
    KAction* redoAllAction = actionCollection()->addAction( QLatin1String( "move_redo_all" ) );
    redoAllAction->setIcon( KIcon( QLatin1String(  "media-skip-forward" ) ) );
    redoAllAction->setText( i18n( "Redo All" ) );
    redoAllAction->setEnabled( false );
    connect( m_scene, SIGNAL(canRedoChanged(bool)), redoAllAction, SLOT(setEnabled(bool)) );
    connect( redoAllAction, SIGNAL(triggered(bool)), m_scene, SLOT(redoAllMove()) );

    // settings menu
    KStandardAction::preferences( this, SLOT(configureSettings()), actionCollection() );
    KStandardAction::configureNotifications( this, SLOT(configureNotifications()), actionCollection() );

    if ( m_KSameMode ) {
        setupGUI( QSize( 576, 384 ) );
        return;
    }

    Kg::difficulty()->addStandardLevelRange(
        KgDifficultyLevel::VeryEasy, KgDifficultyLevel::Hard,
        KgDifficultyLevel::Easy //default
    );
    Kg::difficulty()->addLevel(new KgDifficultyLevel(1000,
        QByteArray( "Custom" ), i18n( "Custom" )
    ));
    KgDifficultyGUI::init(this);
    connect(Kg::difficulty(), SIGNAL(currentLevelChanged(const KgDifficultyLevel*)), SLOT(newGame()));

    setupGUI( QSize( 340, 510 ) );
}

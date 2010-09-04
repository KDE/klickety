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

#include <KAction>
#include <KActionCollection>
#include <KApplication>
#include <KConfigDialog>
#include <KFileDialog>
#include <KGameClock>
#include <KGameDifficulty>
#include <KGameThemeSelector>
#include <KInputDialog>
#include <KLocale>
#include <KMessageBox>
#include <KNotifyConfigWidget>
#include <KScoreDialog>
#include <KStandardAction>
#include <KStandardGameAction>
#include <KStatusBar>
#include <KToggleAction>

#include "bgselector.h"
#include "customgame.h"
#include "gameconfig.h"
#include "gamescene.h"
#include "gameview.h"
#include "mainwindow.h"
#include "settings.h"

MainWindow::MainWindow( QWidget* parent )
: KXmlGuiWindow(parent)
{
    m_scene = new GameScene;
    GameView* view = new GameView( m_scene );
    view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setFrameStyle( QFrame::NoFrame );
    view->setCacheMode( QGraphicsView::CacheBackground );
    setCentralWidget( view );

    m_gameClock = new KGameClock( this, KGameClock::MinSecOnly );
    connect( m_gameClock, SIGNAL(timeChanged(const QString&)), this, SLOT(changeTime(const QString&)) );

    connect( m_scene, SIGNAL(remainCountChanged(int)), this, SLOT(changeRemainCount(int)) );
    connect( m_scene, SIGNAL(gameFinished(int)), this, SLOT(onGameOver(int)) );

    statusBar()->insertItem( i18n( "Pieces: 0" ), 0 );
    statusBar()->insertItem( i18n( "Time: 00:00" ), 1 );

    setupActions();

    loadSettings();
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
    if ( KConfigDialog::showDialog( "settings" ) )
        return;

    KConfigDialog* dialog = new KConfigDialog( this, "settings", Settings::self() );
    dialog->addPage( new GameConfig( dialog ), i18n( "General" ), "games-config-options" );
    dialog->addPage( new KGameThemeSelector( dialog, Settings::self(), KGameThemeSelector::NewStuffDisableDownload ), i18n( "Theme" ), "games-config-theme" );
    dialog->addPage( new BackgroundSelector( dialog ), i18n( "Background" ), "games-config-background" );
    dialog->addPage( new CustomGameConfig( dialog ), i18n( "Custom Game" ), "games-config-custom" );
    connect( dialog, SIGNAL(settingsChanged(const QString&)), this, SLOT(loadSettings()) );
    dialog->setHelp( QString(), "klickety" );
    dialog->show();
}

void MainWindow::customLevelChanged( int key )
{
    Q_UNUSED(key)
    newGame();
}

void MainWindow::levelChanged( KGameDifficulty::standardLevel level )
{
    Q_UNUSED(level)
    Settings::setLevel( KGameDifficulty::level() );
    Settings::self()->writeConfig();
    newGame();
}

void MainWindow::loadSettings()
{
    KGameDifficulty::setLevel( KGameDifficulty::standardLevel( Settings::level() ) );
    m_scene->setRendererTheme( Settings::theme() );
    m_scene->setShowBoundLines( Settings::showBoundLines() );
    m_scene->setEnableAnimation( Settings::enableAnimation() );
    m_scene->setEnableHighlight( Settings::enableHighlight() );
    m_scene->invalidate( m_scene->sceneRect(), QGraphicsScene::BackgroundLayer );
}

void MainWindow::newGame( int gameId )
{
    if ( !confirmAbort() )
        return;

    m_gameClock->restart();
    m_pauseAction->setChecked( false );
    m_pauseAction->setEnabled( true );

    switch ( KGameDifficulty::level() ) {
        case KGameDifficulty::VeryEasy:
            m_scene->startNewGame( 10, 16, 3, gameId );
            break;
        case KGameDifficulty::Easy:
            m_scene->startNewGame( 10, 16, 4, gameId );
            break;
        case KGameDifficulty::Medium:
            m_scene->startNewGame( 10, 16, 5, gameId );
            break;
        case KGameDifficulty::Hard:
            m_scene->startNewGame( 10, 16, 6, gameId );
            break;
        case KGameDifficulty::Configurable:
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
    if ( isPaused )
        m_gameClock->pause();
    else
        m_gameClock->resume();
}

void MainWindow::restartGame()
{
    if ( confirmAbort() ) {
        m_pauseAction->setChecked( false );
        m_pauseAction->setEnabled( true );
        m_scene->restartGame();
    }
}

void MainWindow::loadGame()
{
    QString fileName = KFileDialog::getOpenFileName( KUrl(), "*.klickety", this );
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
    QString fileName = KFileDialog::getSaveFileName( KUrl(), "*.klickety", this );
    if ( fileName.isEmpty() )
        return;
    KConfig config( fileName, KConfig::SimpleConfig );
    KConfigGroup group = config.group( "Savegame" );
    m_scene->saveGame( group );
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
    KScoreDialog d( KScoreDialog::Name, this );
    d.addField( KScoreDialog::Custom1, "Remain pieces", "remains" );
    d.addField( KScoreDialog::Custom2, "Time", "time" );
    d.addLocalizedConfigGroupNames( KGameDifficulty::localizedLevelStrings() );
    d.setConfigGroupWeights( KGameDifficulty::levelWeights() );
    d.hideField( KScoreDialog::Score );
    d.setConfigGroup( KGameDifficulty::localizedLevelString() );
    d.exec();
}

void MainWindow::onGameOver( int remainCount )
{
    m_gameClock->pause();
    m_pauseAction->setEnabled( false );
    KGameDifficulty::setRunning( false );

    KScoreDialog d( KScoreDialog::Name, this );
    d.addField( KScoreDialog::Custom1, "Remain pieces", "remains" );
    d.addField( KScoreDialog::Custom2, "Time", "time" );
    d.addLocalizedConfigGroupNames( KGameDifficulty::localizedLevelStrings() );
    d.setConfigGroupWeights( KGameDifficulty::levelWeights() );
    d.hideField( KScoreDialog::Score );

    QPair<QByteArray, QString> group = KGameDifficulty::localizedLevelString();
    if ( group.first.isEmpty() )
        group = qMakePair( QByteArray( "Custom" ), i18n( "Custom" ) );
        d.setConfigGroup( group );

    KScoreDialog::FieldInfo scoreInfo;
    scoreInfo[KScoreDialog::Custom1].setNum( remainCount );
    scoreInfo[KScoreDialog::Custom2] = m_gameClock->timeString();
    // remainCount*10000000 is much bigger than a usual time seconds
    scoreInfo[KScoreDialog::Score].setNum( remainCount*10000000 + m_gameClock->seconds() );
    if ( d.addScore( scoreInfo, KScoreDialog::LessIsMore ) != 0 )
        d.exec();
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
    KStandardGameAction::load( this, SLOT(loadGame()), actionCollection() );
    KStandardGameAction::restart( this, SLOT(restartGame()), actionCollection() );
    KStandardGameAction::save( this, SLOT(saveGame()), actionCollection() );
    KStandardGameAction::highscores( this, SLOT(showHighscores()), actionCollection() );
    m_pauseAction = KStandardGameAction::pause( this, SLOT(pauseGame(bool)), actionCollection() );
    KStandardGameAction::quit( this, SLOT(close()), actionCollection() );
    KAction* m_newNumGameAction = new KAction( i18n( "New Numbered Game..." ), actionCollection() );
    actionCollection()->addAction( "game_new_numeric", m_newNumGameAction );
    connect( m_newNumGameAction, SIGNAL(triggered(bool)), this, SLOT(newNumGame()) );

    // move menu
    KAction* undoAction = KStandardGameAction::undo( m_scene, SLOT(undoMove()), actionCollection() );
    undoAction->setEnabled( false );
    connect( m_scene, SIGNAL(canUndoChanged(bool)), undoAction, SLOT(setEnabled(bool)) );
    KAction* redoAction = KStandardGameAction::redo( m_scene, SLOT(redoMove()), actionCollection() );
    redoAction->setEnabled( false );
    connect( m_scene, SIGNAL(canRedoChanged(bool)), redoAction, SLOT(setEnabled(bool)) );
    KStandardAction::preferences( this, SLOT(configureSettings()), actionCollection() );
    KStandardAction::configureNotifications( this, SLOT(configureNotifications()), actionCollection() );

    KAction* undoAllAction = actionCollection()->addAction( "move_undo_all" );
    undoAllAction->setIcon( KIcon( "media-skip-backward" ) );
    undoAllAction->setText( i18n( "Undo All" ) );
    undoAllAction->setEnabled( false );
    connect( m_scene, SIGNAL(canUndoChanged(bool)), undoAllAction, SLOT(setEnabled(bool)) );
    connect( undoAllAction, SIGNAL(triggered(bool)), m_scene, SLOT(undoAllMove()) );
    KAction* redoAllAction = actionCollection()->addAction( "move_redo_all" );
    redoAllAction->setIcon( KIcon( "media-skip-forward" ) );
    redoAllAction->setText( i18n( "Redo All" ) );
    redoAllAction->setEnabled( false );
    connect( m_scene, SIGNAL(canRedoChanged(bool)), redoAllAction, SLOT(setEnabled(bool)) );
    connect( redoAllAction, SIGNAL(triggered(bool)), m_scene, SLOT(redoAllMove()) );

    KGameDifficulty::init( this, this, SLOT(levelChanged(KGameDifficulty::standardLevel)),
                           SLOT(customLevelChanged(int)) );
    KGameDifficulty::setRestartOnChange( KGameDifficulty::RestartOnChange );
    KGameDifficulty::addStandardLevel( KGameDifficulty::VeryEasy );
    KGameDifficulty::addStandardLevel( KGameDifficulty::Easy );
    KGameDifficulty::addStandardLevel( KGameDifficulty::Medium );
    KGameDifficulty::addStandardLevel( KGameDifficulty::Hard );
    KGameDifficulty::addStandardLevel( KGameDifficulty::Configurable );

    setupGUI( QSize( 340, 510 ) );
}

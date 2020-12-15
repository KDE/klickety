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

#include "bgselectorconfig.h"
#include "customgameconfig.h"
#include "gameconfig.h"
#include "gamescene.h"
#include "gameview.h"
#include "settings.h"

#include <QAction>
#include <KActionCollection>
#include <KConfigDialog>
#include <QFileDialog>
#include <KGameClock>
#include <KgDifficulty>
#include <KgThemeSelector>
#include <QInputDialog>
#include <KLocalizedString>
#include <KMessageBox>
#include <KScoreDialog>
#include <KStandardAction>
#include <KStandardGameAction>
#include <QStatusBar>
#include <KToggleAction>
#include <QIcon>
#include <QPointer>
#include <QRandomGenerator>

MainWindow::MainWindow( bool kSameMode, QWidget* parent )
: KXmlGuiWindow(parent),
m_kSameMode(kSameMode),
m_gameClock(nullptr),
m_gameScore(0),
m_lastRemainCount(0),
m_statusBarLabelLeft(new QLabel(this)),
m_statusBarLabelRight(new QLabel(this))
{
    m_scene = new GameScene(this);
    GameView* view = new GameView( m_scene );
    view->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    view->setFrameStyle( QFrame::NoFrame );
    view->setCacheMode( QGraphicsView::CacheBackground );
    setCentralWidget( view );

    statusBar()->addPermanentWidget( m_statusBarLabelLeft, 1 );
    statusBar()->addPermanentWidget( m_statusBarLabelRight, 1 );

    if ( m_kSameMode ) {
//         statusBar()->insertItem( i18n( "Colors: XX" ), 1 );
//         statusBar()->insertItem( i18n( "Board: XXXXXX" ), 2 );
        m_statusBarLabelLeft->setText( i18n( "Marked: 0" ) );
        m_statusBarLabelRight->setText( i18n( "Score: 0" ) );
        connect(m_scene, &GameScene::remainCountChanged, this, &MainWindow::changeScore);
        connect(m_scene, &GameScene::markedCountChanged, this, &MainWindow::changeMarkedCount);
    }
    else {
        m_gameClock = new KGameClock( this, KGameClock::MinSecOnly );
        m_statusBarLabelLeft->setText( i18n( "Pieces: 0" ) );
        m_statusBarLabelRight->setText( i18n( "Time: 00:00" ) );
        connect(m_scene, &GameScene::remainCountChanged, this, &MainWindow::changeRemainCount);
        connect(m_gameClock, &KGameClock::timeChanged, this, &MainWindow::changeTime);
    }

    connect(m_scene, &GameScene::gameFinished, this, &MainWindow::onGameOver);

    setupActions();

    loadSettings();

    newGame(QRandomGenerator::global()->bounded(RAND_MAX));
}

MainWindow::~MainWindow()
{
}

void MainWindow::configureSettings()
{
    if ( KConfigDialog::showDialog( QStringLiteral( "settings" ) ) ) {
        return;
    }

    KConfigDialog* dialog = new KConfigDialog( this, QStringLiteral( "settings" ), Settings::self() );
    dialog->addPage( new GameConfig( dialog ), i18n( "General" ), QStringLiteral( "games-config-options" ) );
    dialog->addPage( new KgThemeSelector( m_scene->themeProvider() ), i18n( "Theme" ), QStringLiteral( "games-config-theme" ) );
    dialog->addPage( new BgSelectorConfig( dialog ), i18n( "Background" ), QStringLiteral( "games-config-background" ) );
    if ( !m_kSameMode ) {
        dialog->addPage( new CustomGameConfig( dialog ), i18n( "Custom Game" ), QStringLiteral( "games-config-custom" ) );
    }

    connect(m_scene->themeProvider(), &KgThemeProvider::currentThemeChanged, this, &MainWindow::loadSettings); //setBackgroundType!
    connect(dialog, &KConfigDialog::settingsChanged, this, &MainWindow::loadSettings);
    dialog->show();
}

void MainWindow::loadSettings()
{
    m_scene->setShowBoundLines( Settings::showBoundLines() );
    m_scene->setEnableAnimation( Settings::enableAnimation() );
    m_scene->setEnableHighlight( Settings::enableHighlight() );

    int bgType = 0; // Theme Radio Button
    if ( Settings::radioColor() == true ) {
        bgType = 1;
    } else if ( Settings::radioImage() == true ) {
        bgType = 2;
    }
    m_scene->setBackgroundType( bgType );
}

void MainWindow::newGame( int gameId )
{
    if ( !confirmAbort() ) {
        return;
    }

    m_pauseAction->setChecked( false );
    m_pauseAction->setEnabled( true );

    if ( m_kSameMode ) {
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
    if ( !confirmAbort() ) {
        return;
    }

    bool ok = false;
    int random = QRandomGenerator::global()->bounded(1, RAND_MAX);
    int userGameId = QInputDialog::getInt( this, i18n( "Select Board" ),
                                                i18n( "Select a board number:" ),
                                                random, 1, RAND_MAX, 1,
                                                &ok );
    if ( ok ) {
        newGame( userGameId );
    }
}

void MainWindow::pauseGame( bool isPaused )
{
    m_scene->setPaused( isPaused );
    if ( !m_kSameMode ) {
        if ( isPaused ) {
            m_gameClock->pause();
        } else {
            m_gameClock->resume();
        }
    }
}

void MainWindow::restartGame()
{
    if ( confirmAbort() ) {
        m_pauseAction->setChecked( false );
        m_pauseAction->setEnabled( true );
        if ( m_kSameMode ) {
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
    QString fileName = QFileDialog::getOpenFileName( this, QString(), QString() , i18n( "Klickety Game Files (*.klickety)" ) );
    if ( fileName.isEmpty() || !confirmAbort() ) {
        return;
    }

    m_pauseAction->setChecked( false );
    m_pauseAction->setEnabled( true );
    KConfig config( fileName, KConfig::SimpleConfig );
    KConfigGroup group = config.group( "Savegame" );
    m_scene->loadGame( group );
}

void MainWindow::saveGame()
{
    QString fileName = QFileDialog::getSaveFileName( this, QString(), QString(), i18n( "Klickety Game Files (*.klickety)" ) );
    if ( fileName.isEmpty() ) {
        return;
    }
    KConfig config( fileName, KConfig::SimpleConfig );
    KConfigGroup group = config.group( "Savegame" );
    m_scene->saveGame( group );
}

void MainWindow::changeMarkedCount( int markedCount )
{
    int markedScore = ( markedCount < 2 ) ? 0 : ( ( markedCount - 2 ) * ( markedCount - 2 ) );
    m_statusBarLabelLeft->setText( i18np( "Marked: %2 (1 Point)", "Marked: %2 (%1 Points)", markedScore, markedCount ) );
}

void MainWindow::changeScore( int remainCount )
{
    if ( m_lastRemainCount == 0 ) {
        // new game or restart
        m_lastRemainCount = remainCount;
        m_statusBarLabelRight->setText( i18n( "Score: 0" ) );
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
    m_statusBarLabelRight->setText( i18n( "Score: %1", m_gameScore ) );
    m_lastRemainCount = remainCount;
}

void MainWindow::changeRemainCount( int remainCount )
{
    m_statusBarLabelLeft->setText( i18n( "Pieces: %1", remainCount ) );
}

void MainWindow::changeTime( const QString& newTime )
{
    m_statusBarLabelRight->setText( i18n( "Time: %1", newTime ) );
}

void MainWindow::showHighscores()
{
    if ( m_kSameMode ) {
        QPointer<KScoreDialog> d = new KScoreDialog( KScoreDialog::Name | KScoreDialog::Score, this );
        d->initFromDifficulty(Kg::difficulty(), /*setConfigGroup=*/ false);
        d->setConfigGroup( qMakePair( QByteArray( "KSame" ), i18n( "High Scores" ) ) );
        d->setHiddenConfigGroups( QList<QByteArray>() << "Very Easy" << "Easy" << "Medium" << "Hard" << "Custom" );
        d->exec();
        delete d;
        return;
    }

    QPointer<KScoreDialog> d = new KScoreDialog( KScoreDialog::Name, this );
    d->addField( KScoreDialog::Custom1, i18n( "Remaining pieces" ), QStringLiteral( "remains" ) );
    d->addField( KScoreDialog::Custom2, i18n( "Time" ), QStringLiteral( "time" ) );
    d->initFromDifficulty(Kg::difficulty(), /*setConfigGroup=*/ true);
    d->setHiddenConfigGroups( QList<QByteArray>() << "KSame" );
    d->hideField( KScoreDialog::Score );
    d->exec();
    delete d;
}

void MainWindow::onGameOver( int remainCount )
{
    m_pauseAction->setEnabled( false );

    if ( m_kSameMode ) {
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

        if ( d->addScore( scoreInfo ) != 0 ) {
            d->exec();
}
        delete d;
        return;
    }

    m_gameClock->pause();

    QPointer<KScoreDialog> d = new KScoreDialog( KScoreDialog::Name, this );
    d->addField( KScoreDialog::Custom1, i18n( "Remaining pieces" ), QStringLiteral( "remains" ) );
    d->addField( KScoreDialog::Custom2, i18n( "Time" ), QStringLiteral( "time" ) );
    d->initFromDifficulty(Kg::difficulty(), /*setConfigGroup=*/ true);
    d->setHiddenConfigGroups( QList<QByteArray>() << "KSame" );
    d->hideField( KScoreDialog::Score );

    KScoreDialog::FieldInfo scoreInfo;
    scoreInfo[KScoreDialog::Custom1].setNum( remainCount );
    scoreInfo[KScoreDialog::Custom2] = m_gameClock->timeString();
    // remainCount*10000000 is much bigger than a usual time seconds
    scoreInfo[KScoreDialog::Score].setNum( remainCount*10000000 + m_gameClock->seconds() );
    if ( d->addScore( scoreInfo, KScoreDialog::LessIsMore ) != 0 ) {
        d->exec();
    }
    delete d;
}

bool MainWindow::confirmAbort()
{
    return m_scene->isGameFinished() || ( KMessageBox::questionYesNo( this, i18n( "Do you want to resign?" ),
        i18n( "New Game" ), KGuiItem( i18n( "Resign" ) ), KStandardGuiItem::cancel() ) == KMessageBox::Yes );
}

void MainWindow::setupActions()
{
    // game menu
    KStandardGameAction::gameNew(this, &MainWindow::newGame, actionCollection());
    if ( !m_kSameMode ) {
        KStandardGameAction::load(this, &MainWindow::loadGame, actionCollection());
        KStandardGameAction::save(this, &MainWindow::saveGame, actionCollection());
    }
    KStandardGameAction::restart(this, &MainWindow::restartGame, actionCollection());
    KStandardGameAction::highscores(this, &MainWindow::showHighscores, actionCollection());
    m_pauseAction = KStandardGameAction::pause(this, &MainWindow::pauseGame, actionCollection());
    KStandardGameAction::quit( this, &MainWindow::close, actionCollection());
    QAction * m_newNumGameAction = new QAction( i18n( "New Numbered Game..." ), actionCollection() );
    actionCollection()->addAction( QStringLiteral( "game_new_numeric" ), m_newNumGameAction );
    connect(m_newNumGameAction, &QAction::triggered, this, &MainWindow::newNumGame);

    KToggleAction * soundAction = new KToggleAction(QIcon::fromTheme(QStringLiteral("speaker")), i18n("Play Sounds"), this);
    soundAction->setChecked(Settings::enableSounds());
    actionCollection()->addAction(QStringLiteral("sounds"), soundAction);
    connect(soundAction, &KToggleAction::triggered, this, &MainWindow::setSoundsEnabled);

    // move menu
    QAction * undoAction = KStandardGameAction::undo(m_scene, &GameScene::undoMove, actionCollection());
    undoAction->setEnabled( false );
    connect(m_scene, &GameScene::canUndoChanged, undoAction, &QAction::setEnabled);
    QAction * redoAction = KStandardGameAction::redo(m_scene, &GameScene::redoMove, actionCollection());
    redoAction->setEnabled( false );
    connect(m_scene, &GameScene::canRedoChanged, redoAction, &QAction::setEnabled);

    QAction * undoAllAction = actionCollection()->addAction( QStringLiteral( "move_undo_all" ) );
    undoAllAction->setIcon( QIcon::fromTheme( QStringLiteral( "media-skip-backward" ) ) );
    undoAllAction->setText( i18n( "Undo All" ) );
    undoAllAction->setEnabled( false );
    connect(m_scene, &GameScene::canUndoChanged, undoAllAction, &QAction::setEnabled);
    connect(undoAllAction, &QAction::triggered, m_scene, &GameScene::undoAllMove);
    QAction * redoAllAction = actionCollection()->addAction( QStringLiteral( "move_redo_all" ) );
    redoAllAction->setIcon( QIcon::fromTheme( QStringLiteral( "media-skip-forward" ) ) );
    redoAllAction->setText( i18n( "Redo All" ) );
    redoAllAction->setEnabled( false );
    connect(m_scene, &GameScene::canRedoChanged, redoAllAction, &QAction::setEnabled);
    connect(redoAllAction, &QAction::triggered, m_scene, &GameScene::redoAllMove);

    // settings menu
    KStandardAction::preferences(this, &MainWindow::configureSettings, actionCollection());

    if ( m_kSameMode ) {
        Kg::difficulty()->addLevel(new KgDifficultyLevel(0,
            QByteArray( "KSame" ), i18n( "High Scores" )
        ));
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
    connect(Kg::difficulty(), &KgDifficulty::currentLevelChanged,
            this, [this]() { newGame(QRandomGenerator::global()->bounded(RAND_MAX)); });

    setupGUI( QSize( 340, 510 ) );
}

void MainWindow::setSoundsEnabled( bool enabled )
{
    Settings::setEnableSounds( enabled );
    Settings::self()->save();
}

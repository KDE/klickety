/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "mainwindow.h"

#include "bgselectorconfig.h"
#include "customgameconfig.h"
#include "gameconfig.h"
#include "gamescene.h"
#include "gameview.h"
#include "settings.h"

#include <KGameHighScoreDialog>

#include <QAction>
#include <KActionCollection>
#include <KConfigDialog>
#include <QFileDialog>
#include <KGameClock>
#include <KGameDifficulty>
#include <KGameThemeSelector>
#include <QInputDialog>
#include <KLocalizedString>
#include <KMessageBox>
#include <KStandardAction>
#include <KGameStandardAction>
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

    newGame();
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
    dialog->addPage( new GameConfig( dialog ), i18nc( "@title:tab", "General" ), QStringLiteral( "games-config-options" ) );
    dialog->addPage( new KGameThemeSelector( m_scene->themeProvider() ), i18n( "Theme" ), QStringLiteral( "games-config-theme" ) );
    dialog->addPage( new BgSelectorConfig( dialog ), i18nc( "@title:tab", "Background" ), QStringLiteral( "games-config-background" ) );
    if ( !m_kSameMode ) {
        dialog->addPage( new CustomGameConfig( dialog ), i18nc( "@title:tab", "Custom Game" ), QStringLiteral( "games-config-custom" ) );
    }

    connect(m_scene->themeProvider(), &KGameThemeProvider::currentThemeChanged, this, &MainWindow::loadSettings); //setBackgroundType!
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

void MainWindow::newGame()
{
    newGameWithId(QRandomGenerator::global()->bounded(RAND_MAX));
}

void MainWindow::newGameWithId( int gameId )
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

    switch ( KGameDifficulty::globalLevel() ) {
        case KGameDifficultyLevel::VeryEasy:
            m_scene->startNewGame( 10, 16, 3, gameId );
            break;
        case KGameDifficultyLevel::Easy:
            m_scene->startNewGame( 10, 16, 4, gameId );
            break;
        case KGameDifficultyLevel::Medium:
            m_scene->startNewGame( 10, 16, 5, gameId );
            break;
        case KGameDifficultyLevel::Hard:
            m_scene->startNewGame( 10, 16, 6, gameId );
            break;
        case KGameDifficultyLevel::Custom:
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
    int userGameId = QInputDialog::getInt( this, i18nc("@title:window", "Select Board" ),
                                                i18n( "Select a board number:" ),
                                                random, 1, RAND_MAX, 1,
                                                &ok );
    if ( ok ) {
        newGameWithId(userGameId);
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
    KConfigGroup group = config.group( QStringLiteral("Savegame") );
    m_scene->loadGame( group );
}

void MainWindow::saveGame()
{
    QString fileName = QFileDialog::getSaveFileName( this, QString(), QString(), i18n( "Klickety Game Files (*.klickety)" ) );
    if ( fileName.isEmpty() ) {
        return;
    }
    KConfig config( fileName, KConfig::SimpleConfig );
    KConfigGroup group = config.group( QStringLiteral("Savegame") );
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
        QPointer<KGameHighScoreDialog> d = new KGameHighScoreDialog( KGameHighScoreDialog::Name | KGameHighScoreDialog::Score, this );
        d->initFromDifficulty(KGameDifficulty::global(), /*setConfigGroup=*/ false);
        d->setConfigGroup( qMakePair( QByteArray( "KSame" ), i18n( "High Scores" ) ) );
        d->setHiddenConfigGroups( QList<QByteArray>() << "Very Easy" << "Easy" << "Medium" << "Hard" << "Custom" );
        d->exec();
        delete d;
        return;
    }

    QPointer<KGameHighScoreDialog> d = new KGameHighScoreDialog( KGameHighScoreDialog::Name, this );
    d->addField( KGameHighScoreDialog::Custom1, i18n( "Remaining pieces" ), QStringLiteral( "remains" ) );
    d->addField( KGameHighScoreDialog::Custom2, i18n( "Time" ), QStringLiteral( "time" ) );
    d->initFromDifficulty(KGameDifficulty::global(), /*setConfigGroup=*/ true);
    d->setHiddenConfigGroups( QList<QByteArray>() << "KSame" );
    d->hideField( KGameHighScoreDialog::Score );
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

        QPointer<KGameHighScoreDialog> d = new KGameHighScoreDialog( KGameHighScoreDialog::Name | KGameHighScoreDialog::Score, this );
        d->initFromDifficulty(KGameDifficulty::global(), /*setConfigGroup=*/ false);
        d->setConfigGroup( qMakePair( QByteArray( "KSame" ), i18n( "High Scores" ) ) );
        d->setHiddenConfigGroups( QList<QByteArray>() << "Very Easy" << "Easy" << "Medium" << "Hard" << "Custom" );

        KGameHighScoreDialog::FieldInfo scoreInfo;
        scoreInfo[ KGameHighScoreDialog::Score ].setNum( m_gameScore );

        if ( d->addScore( scoreInfo ) != 0 ) {
            d->exec();
}
        delete d;
        return;
    }

    m_gameClock->pause();

    QPointer<KGameHighScoreDialog> d = new KGameHighScoreDialog( KGameHighScoreDialog::Name, this );
    d->addField( KGameHighScoreDialog::Custom1, i18n( "Remaining pieces" ), QStringLiteral( "remains" ) );
    d->addField( KGameHighScoreDialog::Custom2, i18n( "Time" ), QStringLiteral( "time" ) );
    d->initFromDifficulty(KGameDifficulty::global(), /*setConfigGroup=*/ true);
    d->setHiddenConfigGroups( QList<QByteArray>() << "KSame" );
    d->hideField( KGameHighScoreDialog::Score );

    KGameHighScoreDialog::FieldInfo scoreInfo;
    scoreInfo[KGameHighScoreDialog::Custom1].setNum( remainCount );
    scoreInfo[KGameHighScoreDialog::Custom2] = m_gameClock->timeString();
    // remainCount*10000000 is much bigger than a usual time seconds
    scoreInfo[KGameHighScoreDialog::Score].setNum( remainCount*10000000 + m_gameClock->seconds() );
    if ( d->addScore( scoreInfo, KGameHighScoreDialog::LessIsMore ) != 0 ) {
        d->exec();
    }
    delete d;
}

bool MainWindow::confirmAbort()
{
    return m_scene->isGameFinished() ||
        ( KMessageBox::questionTwoActions( this,
                                      i18n( "Do you want to resign?" ),
                                      i18nc( "@title:window", "New Game" ),
                                      KGuiItem( i18nc("@action:button", "Resign") ),
                                      KStandardGuiItem::cancel() )
          == KMessageBox::PrimaryAction );
}

void MainWindow::setupActions()
{
    // game menu
    KGameStandardAction::gameNew(this, &MainWindow::newGame, actionCollection());
    if ( !m_kSameMode ) {
        KGameStandardAction::load(this, &MainWindow::loadGame, actionCollection());
        KGameStandardAction::save(this, &MainWindow::saveGame, actionCollection());
    }
    KGameStandardAction::restart(this, &MainWindow::restartGame, actionCollection());
    KGameStandardAction::highscores(this, &MainWindow::showHighscores, actionCollection());
    m_pauseAction = KGameStandardAction::pause(this, &MainWindow::pauseGame, actionCollection());
    KGameStandardAction::quit( this, &MainWindow::close, actionCollection());
    QAction * m_newNumGameAction = new QAction( i18nc( "@action", "New Numbered Game…" ), actionCollection() );
    actionCollection()->addAction( QStringLiteral( "game_new_numeric" ), m_newNumGameAction );
    connect(m_newNumGameAction, &QAction::triggered, this, &MainWindow::newNumGame);

    auto *soundAction = new KToggleAction(QIcon::fromTheme(QStringLiteral("speaker")),
                                          i18nc("@option:check", "Play Sounds"), this);
    soundAction->setChecked(Settings::enableSounds());
    actionCollection()->addAction(QStringLiteral("sounds"), soundAction);
    connect(soundAction, &KToggleAction::triggered, this, &MainWindow::setSoundsEnabled);

    // move menu
    QAction * undoAction = KGameStandardAction::undo(m_scene, &GameScene::undoMove, actionCollection());
    undoAction->setEnabled( false );
    connect(m_scene, &GameScene::canUndoChanged, undoAction, &QAction::setEnabled);
    QAction * redoAction = KGameStandardAction::redo(m_scene, &GameScene::redoMove, actionCollection());
    redoAction->setEnabled( false );
    connect(m_scene, &GameScene::canRedoChanged, redoAction, &QAction::setEnabled);

    QAction * undoAllAction = actionCollection()->addAction( QStringLiteral( "move_undo_all" ) );
    undoAllAction->setIcon( QIcon::fromTheme( QStringLiteral( "media-skip-backward" ) ) );
    undoAllAction->setText( i18nc( "@action", "Undo All" ) );
    undoAllAction->setEnabled( false );
    connect(m_scene, &GameScene::canUndoChanged, undoAllAction, &QAction::setEnabled);
    connect(undoAllAction, &QAction::triggered, m_scene, &GameScene::undoAllMove);
    QAction * redoAllAction = actionCollection()->addAction( QStringLiteral( "move_redo_all" ) );
    redoAllAction->setIcon( QIcon::fromTheme( QStringLiteral( "media-skip-forward" ) ) );
    redoAllAction->setText( i18nc( "@action", "Redo All" ) );
    redoAllAction->setEnabled( false );
    connect(m_scene, &GameScene::canRedoChanged, redoAllAction, &QAction::setEnabled);
    connect(redoAllAction, &QAction::triggered, m_scene, &GameScene::redoAllMove);

    // settings menu
    KStandardAction::preferences(this, &MainWindow::configureSettings, actionCollection());

    if ( m_kSameMode ) {
        KGameDifficulty::global()->addLevel(new KGameDifficultyLevel(0,
            QByteArray( "KSame" ), i18n( "High Scores" )
        ));
        setupGUI( QSize( 576, 384 ) );
        return;
    }

    KGameDifficulty::global()->addStandardLevelRange(
        KGameDifficultyLevel::VeryEasy, KGameDifficultyLevel::Hard,
        KGameDifficultyLevel::Easy //default
    );
    KGameDifficulty::global()->addLevel(new KGameDifficultyLevel(1000,
        QByteArray( "Custom" ), i18n( "Custom" )
    ));
    KGameDifficultyGUI::init(this);
    connect(KGameDifficulty::global(), &KGameDifficulty::currentLevelChanged,
            this, &MainWindow::newGame);

    setupGUI( QSize( 340, 510 ) );
}

void MainWindow::setSoundsEnabled( bool enabled )
{
    Settings::setEnableSounds( enabled );
    Settings::self()->save();
}

#include "moc_mainwindow.cpp"

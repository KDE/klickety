/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "gamescene.h"

#include "piece.h"
#include "settings.h"
#include "undo.h"
// KDEGames
#include <KGamePopupItem>
#include <KGameTheme>
#include <KGameThemeProvider>
// KF
#include <KConfigGroup>
#include <KLocalizedString>
// Qt
#include <QRandomGenerator>
#include <QEasingCurve>
#include <QGraphicsColorizeEffect>
#include <QPainter>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QStandardPaths>

static KGameThemeProvider* provider()
{
    //TODO: Do we want to store separate theme choices for Klickety and KSame?
    const QString defaultTheme =
        Settings::self()->config()->name() == QLatin1String("ksamerc")
        ? QStringLiteral("ksame") : QStringLiteral("default");
    KGameThemeProvider* prov = new KGameThemeProvider;
    prov->discoverThemes(QStringLiteral("themes"), defaultTheme);
    return prov;
}

GameScene::GameScene( QObject* parent )
: QGraphicsScene(parent),
m_renderer(provider()),
m_messenger(new KGamePopupItem),
m_showBoundLines(true),
m_enableAnimation(true),
m_enableHighlight(true),
m_backgroundType(0),
PWC(0),
PHC(0),
m_colorCount(0),
m_gameId(QRandomGenerator::global()->bounded(RAND_MAX)),
m_isPaused(false),
m_isFinished(false),
m_animation(new QSequentialAnimationGroup),
m_soundRemove(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("sounds/klickety/remove.ogg")), this),
m_soundGameFinished(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("sounds/klickety/game-finished.ogg")), this)
{
    connect(&m_undoStack, &QUndoStack::canUndoChanged, this, &GameScene::canUndoChanged);
    connect(&m_undoStack, &QUndoStack::canRedoChanged, this, &GameScene::canRedoChanged);
    connect(this, &GameScene::sceneRectChanged, this, &GameScene::resize);

    connect(m_animation, &QSequentialAnimationGroup::finished, this, &GameScene::updateScene);

    // init messenger
    m_messenger->setMessageOpacity( 0.8 );
    m_messenger->setMessageTimeout( 0 );
    m_messenger->setHideOnMouseClick( false );
    addItem( m_messenger );
    m_messenger->forceHide();
}

GameScene::~GameScene()
{
    qDeleteAll( m_pieces );
    delete m_animation;
}

void GameScene::startNewGame( int pwc, int phc, int colorCount, int gameId )
{
    PWC = pwc;
    PHC = phc;
    m_colorCount = colorCount;
    m_gameId = gameId;
    m_isPaused = false;
    m_isFinished = false;
    m_undoStack.clear();

    for (auto & piece : std::as_const(m_pieces)) {
        removeItem( piece );
    }
    qDeleteAll( m_pieces );
    m_pieces.clear();

    // hide messenger if any
    m_messenger->forceHide();

    // create a default pen
    QPen pen;
    pen.setStyle( Qt::SolidLine );
    pen.setWidth( 1 );
    pen.setBrush( Qt::black );
    pen.setCapStyle( Qt::RoundCap );
    pen.setJoinStyle( Qt::RoundJoin );

    QRandomGenerator s( gameId );
    for ( int j = 0; j < PHC; ++j ) {
        for ( int i = 0; i < PWC; ++i ) {
            // piece item
            Piece* item = new Piece( &m_renderer, i, j, s.bounded( m_colorCount ) );
            connect(item, &Piece::pieceClicked, this, &GameScene::removePieces);
            connect(item, &Piece::pieceHovered, this, &GameScene::highlightPieces);
            connect(item, &Piece::pieceUnhovered, this, &GameScene::unhighlightPieces);
            m_pieces << item;
            addItem( item );

            // set up highlight effects
            item->m_highlighter->setParentItem( item );
            item->m_highlighter->hide();

            // bound line item
            item->m_rightLine->setPen( pen );
            item->m_bottomLine->setPen( pen );
            item->m_rightLine->setParentItem( item );
            item->m_bottomLine->setParentItem( item );
            item->m_rightLine->hide();
            item->m_bottomLine->hide();
        }
    }

    Q_EMIT remainCountChanged( pwc * phc );

    updateScene();
}

void GameScene::loadGame( const KConfigGroup& config )
{
    int pwc = config.readEntry( "PWC", -1 );
    int phc = config.readEntry( "PHC", -1 );
    int colorCount = config.readEntry( "ColorCount", -1 );
    int gameId = config.readEntry( "GameId", -1 );
    if ( pwc == -1 || phc == -1 || colorCount == -1 || gameId == -1 ) {
        qWarning() << "Unexpected game parameters.";
        return;
    }
    startNewGame( pwc, phc, colorCount, gameId );

    int moveCount = config.readEntry( "MoveCount", 0 );
    int undoIndex = config.readEntry( "UndoIndex", 0 );
    if ( undoIndex > moveCount ) {
        qWarning() << "Unexpected undo history structure.";
        return;
    }

    // disable animation temporarily
    bool enableAnimationOld = m_enableAnimation;
    setEnableAnimation( false );

    // execute the history
    for ( int i = 0; i < moveCount; ++i ) {
        QList<int> move = config.readEntry( QStringLiteral( "Move_%1" ).arg( i ), QList<int>() );
        if ( move.count() != 2 ) {
            qWarning() << "Unexpected undo command structure.";
            return;
        }
        int x = move.at( 0 );
        int y = move.at( 1 );
        if ( x < 0 || x >= pwc || y < 0 || y >= phc ) {
            qWarning() << "Unexpected undo command logic. Skip it.";
            continue;
        }

        removePieces( x, y );
    }

    // undo
    for ( int i = 0; i < moveCount - undoIndex; ++i ) {
        undoMove();
    }

    setEnableAnimation( enableAnimationOld );

    Q_EMIT remainCountChanged( currentRemainCount() );
}

void GameScene::saveGame( KConfigGroup& config ) const
{
    config.writeEntry( "PWC", PWC );
    config.writeEntry( "PHC", PHC );
    config.writeEntry( "ColorCount", m_colorCount );
    config.writeEntry( "GameId", m_gameId );

    // save undostack
    int moveCount = m_undoStack.count();
    int undoIndex = m_undoStack.index();
    config.writeEntry( "MoveCount", moveCount );
    config.writeEntry( "UndoIndex", undoIndex );
    for ( int i = 0; i < moveCount; ++i ) {
        const QUndoCommand* cmd = m_undoStack.command( i );
        // the first child should be the user click
        const QUndoCommand* click = cmd->child( 0 );
        if ( click->id() != ID_HIDEPIECE ) {
            qWarning() << "Unexpected command id.";
            return;
        }
        Piece* clickPiece = static_cast<const HidePiece*>(click)->m_piece;
        QList<int> move;
        move << clickPiece->m_x << clickPiece->m_y;
        config.writeEntry( QStringLiteral( "Move_%1" ).arg( i ), move );
    }
}

void GameScene::restartGame()
{
    startNewGame( PWC, PHC, m_colorCount, m_gameId );
}

void GameScene::setPaused( bool isPaused )
{
    if ( m_isPaused == isPaused ) {
        return;
    }

    m_isPaused = isPaused;

    // hide or unhide all the enabled pieces
    for ( int j = 0; j < PHC; ++j ) {
        for ( int i = 0; i < PWC; ++i ) {
            Piece* item = m_pieces[j*PWC+i];
            if ( item->isEnabled() ) {
                item->setVisible( !m_isPaused );
            }
        }
    }

    if ( m_isPaused ) {
        m_messenger->showMessage( i18nc( "@info:status", "Paused" ), KGamePopupItem::Center );
        Q_EMIT canUndoChanged( false );
        Q_EMIT canRedoChanged( false );
    }
    else {
        m_messenger->forceHide();
        Q_EMIT canUndoChanged( m_undoStack.canUndo() );
        Q_EMIT canRedoChanged( m_undoStack.canRedo() );
    }
}

bool GameScene::isGameFinished() const
{
    if ( m_pieces.isEmpty() || m_undoStack.isClean() ) {
        return true;
    }

    for ( int j = 0; j < PHC; ++j ) {
        for ( int i = 0; i < PWC; ++i ) {
            Piece* item = m_pieces[j*PWC+i];
            // check same color neighbors, rightside and downside
            if ( !item->isEnabled() ) {
                continue;
            }
            int rightX = i + 1;
            int downY = j + 1;
            if ( rightX < PWC && m_pieces[j*PWC+rightX]->isEnabled()
                && m_pieces[j*PWC+rightX]->m_color == item->m_color ) {
                return false;
            }
            if ( downY < PHC && m_pieces[downY*PWC+i]->isEnabled()
                && m_pieces[downY*PWC+i]->m_color == item->m_color ) {
                return false;
            }
        }
    }
    return true;
}

KGameThemeProvider* GameScene::themeProvider() const
{
    return m_renderer.themeProvider();
}

void GameScene::setBackgroundType( int type )
{
    m_backgroundType = type;
    // update background immediately
    invalidate( sceneRect(), QGraphicsScene::BackgroundLayer );
}

void GameScene::setShowBoundLines( bool isShowing )
{
    if ( m_showBoundLines != isShowing ) {
        m_showBoundLines = isShowing;
        // update bound lines immediately
        updateBoundLines();
    }
}

void GameScene::setEnableAnimation( bool isEnabled )
{
    m_enableAnimation = isEnabled;
}

void GameScene::setEnableHighlight( bool isEnabled )
{
    m_enableHighlight = isEnabled;
}

void GameScene::undoMove()
{
    unhighlightPieces( m_currentlyHoveredPieceX, m_currentlyHoveredPieceY );
    m_undoStack.undo();
    Q_EMIT remainCountChanged( currentRemainCount() );
    updateScene();
}

void GameScene::redoMove()
{
    m_undoStack.redo();
    Q_EMIT remainCountChanged( currentRemainCount() );
    updateScene();
}

void GameScene::undoAllMove()
{
    while ( m_undoStack.canUndo() ) {
        m_undoStack.undo();
    }
    Q_EMIT remainCountChanged( currentRemainCount() );
    updateScene();
}

void GameScene::redoAllMove()
{
    while ( m_undoStack.canRedo() ) {
        m_undoStack.redo();
    }
    Q_EMIT remainCountChanged( currentRemainCount() );
    updateScene();
}

void GameScene::checkGameFinished()
{
    int remain = currentRemainCount();
    Q_EMIT remainCountChanged( remain );
    bool finished = isGameFinished();
    if ( finished && m_isFinished != finished ) {
        if (Settings::enableSounds()) {
            m_soundGameFinished.start();
        }
        m_messenger->showMessage( i18nc( "@info:status", "Game finished" ) , KGamePopupItem::Center );
        Q_EMIT canUndoChanged( false );
        Q_EMIT canRedoChanged( false );
        Q_EMIT gameFinished( remain );
    }
    m_isFinished = finished;
}

void GameScene::traverseNeighbors( int x, int y, int color, bool (GameScene::*func)(Piece*) )
{
    if ( x < 0 || x >= PWC || y < 0 || y >= PHC ) {
        return;
    }

    int index = y * PWC + x;
    if ( m_pieces[index]->m_color == color ) {
        if ( (this->*func)( m_pieces[index] ) ) {
            traverseNeighbors( x-1, y, color, func );// check left neighbor
            traverseNeighbors( x, y-1, color, func );// check up neighbor
            traverseNeighbors( x+1, y, color, func );// check right neighbor
            traverseNeighbors( x, y+1, color, func );// check down neighbor
        }
    }
}

bool GameScene::highlightPiece( Piece* p )
{
    if ( !p->isEnabled() || p->m_highlighter->isVisible() ) {
        return false;
    }
    p->m_highlighter->show();
    return true;
}

bool GameScene::unhighlightPiece( Piece* p )
{
    if ( !p->isEnabled() || !p->m_highlighter->isVisible() ) {
        return false;
    }
    p->m_highlighter->hide();
    return true;
}

bool GameScene::removePiece( Piece* p )
{
    if ( !p->isEnabled() ) {
        return false;
    }
    m_undoStack.push( new HidePiece( p ) );
    return true;
}

bool GameScene::canRemovePiece( int x, int y )
{
    int index = y * PWC + x;
    int color = m_pieces[index]->m_color;

    int leftX = x - 1;
    int rightX = x + 1;
    int upY = y - 1;
    int downY = y + 1;
    return ( leftX >= 0 && m_pieces[y*PWC+leftX]->m_color == color && m_pieces[y*PWC+leftX]->isEnabled() )
        || ( rightX < PWC && m_pieces[y*PWC+rightX]->m_color == color && m_pieces[y*PWC+rightX]->isEnabled() )
        || ( upY >= 0 && m_pieces[upY*PWC+x]->m_color == color && m_pieces[upY*PWC+x]->isEnabled() )
        || ( downY < PHC && m_pieces[downY*PWC+x]->m_color == color && m_pieces[downY*PWC+x]->isEnabled() );
}

void GameScene::highlightPieces( int x, int y )
{
    m_currentlyHoveredPieceX = x;
    m_currentlyHoveredPieceY = y;
    if ( !m_enableHighlight ) {
        return;
    }

    if ( x < 0 || x >= PWC || y < 0 || y >= PHC ) {
        return;
    }

    if ( !canRemovePiece( x, y ) ) {
        return;
    }

    int index = y * PWC + x;
    m_pieces[index]->m_highlighter->show();
    traverseNeighbors( x-1, y, m_pieces[index]->m_color, &GameScene::highlightPiece );// check left neighbor
    traverseNeighbors( x, y-1, m_pieces[index]->m_color, &GameScene::highlightPiece );// check up neighbor
    traverseNeighbors( x+1, y, m_pieces[index]->m_color, &GameScene::highlightPiece );// check right neighbor
    traverseNeighbors( x, y+1, m_pieces[index]->m_color, &GameScene::highlightPiece );// check down neighbor

    Q_EMIT markedCountChanged( currentMarkedCount() );
}

void GameScene::unhighlightPieces( int x, int y )
{
    if ( x < 0 || x >= PWC || y < 0 || y >= PHC ) {
        return;
    }

    if ( !canRemovePiece( x, y ) ) {
        return;
    }

    int index = y * PWC + x;
    m_pieces[index]->m_highlighter->hide();

    traverseNeighbors( x-1, y, m_pieces[index]->m_color, &GameScene::unhighlightPiece );// check left neighbor
    traverseNeighbors( x, y-1, m_pieces[index]->m_color, &GameScene::unhighlightPiece );// check up neighbor
    traverseNeighbors( x+1, y, m_pieces[index]->m_color, &GameScene::unhighlightPiece );// check right neighbor
    traverseNeighbors( x, y+1, m_pieces[index]->m_color, &GameScene::unhighlightPiece );// check down neighbor

    Q_EMIT markedCountChanged( currentMarkedCount() );
}

void GameScene::removePieces( int x, int y )
{
    if ( x < 0 || x >= PWC || y < 0 || y >= PHC ) {
        return;
    }

    if ( !canRemovePiece( x, y ) ) {
        return;
    }

    // If the animation is running we need to emit here the "old"
    // remaining count otherwise the remainCountChanged won't
    // be emitted until the "next" animation finishes meaning
    // that the scores get calculated wrongly because it thinks
    // you removed more pieces in one go that you really did
    if (m_animation->state() == QAbstractAnimation::Running) {
        Q_EMIT remainCountChanged( currentRemainCount() );
    }
    // unhighlight pieces
    unhighlightPieces( x, y );

    int index = y * PWC + x;
    m_undoStack.beginMacro( QStringLiteral( "Remove pieces" ) );
    m_undoStack.push( new HidePiece( m_pieces[index] ) );

    traverseNeighbors( x-1, y, m_pieces[index]->m_color, &GameScene::removePiece );// check left neighbor
    traverseNeighbors( x, y-1, m_pieces[index]->m_color, &GameScene::removePiece );// check up neighbor
    traverseNeighbors( x+1, y, m_pieces[index]->m_color, &GameScene::removePiece );// check right neighbor
    traverseNeighbors( x, y+1, m_pieces[index]->m_color, &GameScene::removePiece );// check down neighbor

    const int elementsSize1 = sceneRect().width() / PWC;
    const int elementsSize2 = sceneRect().height() / PHC;
    const int elementsSize = qMin( elementsSize1, elementsSize2 );

    // horizontal center
    int gameAreaWidth = PWC * elementsSize;
    int xShift = ( sceneRect().width() - gameAreaWidth ) / 2;
    // vertical center
    int gameAreaHeight = PHC * elementsSize;
    int yShift = ( sceneRect().height() - gameAreaHeight ) / 2;

    QParallelAnimationGroup* gravityAnimationGroup = nullptr;
    QParallelAnimationGroup* removeColumnsAnimationGroup = nullptr;
    if ( m_enableAnimation ) {
        // Clearing next line will trigger checkGameFinished but we don't want that since the user just clicked
        // on some more tiles to remove, we will trigger another check game finished at the end of this function
        // either directly or via connection the animation // finished signal again to checkGameFinished
        disconnect(m_animation, &QSequentialAnimationGroup::finished, this, &GameScene::checkGameFinished);
        // remove previous animations if any
        m_animation->clear();
        gravityAnimationGroup = new QParallelAnimationGroup;
        removeColumnsAnimationGroup = new QParallelAnimationGroup;
    }

    // gravity
    for ( int i = 0; i < PWC; ++i ) {
        bool mayNeedMove = false;
        int floorRow = PHC - 1;
        for ( int j = PHC-1; j >= 0; --j ) {
            if ( !m_pieces[ j * PWC + i ]->isEnabled() && !mayNeedMove ) {
                // found the hidden piece
                mayNeedMove = true;
                floorRow = j;
            }
            else if ( m_pieces[ j * PWC + i ]->isEnabled() && mayNeedMove ) {
                // swap the visible one down to floorRow
                Piece* visiblePiece = m_pieces[ j * PWC + i ];
                Piece* hiddenPiece = m_pieces[ floorRow * PWC + i ];
                const QPointF oldpos( xShift + visiblePiece->m_x * elementsSize, yShift + visiblePiece->m_y * elementsSize );
                const QPointF newpos = hiddenPiece->pos();

                m_undoStack.push( new SwapPiece( &m_pieces[j*PWC+i], &m_pieces[floorRow*PWC+i], oldpos, newpos ) );

                if ( m_enableAnimation ) {
                    // restore old position for proper animation beginning state
                    visiblePiece->setPos( oldpos );

                    // 300ms animation is fast enough to prevent a user's resizing during it
                    QPropertyAnimation* animation = new QPropertyAnimation( visiblePiece, "pos", this );
                    animation->setStartValue( oldpos );
                    animation->setEndValue( newpos );
                    animation->setDuration( 250 );
                    animation->setEasingCurve( QEasingCurve::InQuad );
                    gravityAnimationGroup->addAnimation( animation );
                }

                --floorRow;
            }
        }
    }

    // remove empty columns
    bool mayNeedMove = false;
    int floorCol = 0;
    for ( int i = 0; i < PWC; ++i ) {
        if ( !m_pieces[ (PHC-1) * PWC + i ]->isEnabled() && !mayNeedMove ) {
            // found the empty column
            mayNeedMove = true;
            floorCol = i;
        }
        else if ( m_pieces[ (PHC-1) * PWC + i ]->isEnabled() && mayNeedMove ) {
            // swap the visible column down to floorCol
            for ( int j = PHC-1; j >= 0; --j ) {
                if ( m_pieces[ j * PWC + i ]->isEnabled() ) {
                    Piece* visiblePiece = m_pieces[ j * PWC + i ];
                    Piece* hiddenPiece = m_pieces[ j * PWC + floorCol ];
                    const QPointF oldpos( xShift + visiblePiece->m_x * elementsSize, yShift + visiblePiece->m_y * elementsSize );
                    const QPointF newpos = hiddenPiece->pos();

                    m_undoStack.push( new SwapPiece( &m_pieces[j*PWC+i], &m_pieces[j*PWC+floorCol], oldpos, newpos ) );

                    if ( m_enableAnimation ) {
                        // restore old position for proper animation beginning state
                        visiblePiece->setPos( oldpos );

                        // 300ms animation is fast enough to prevent a user's resizing during it
                        QPropertyAnimation* animation = new QPropertyAnimation( visiblePiece, "pos", this );
                        animation->setStartValue( oldpos );
                        animation->setEndValue( newpos );
                        animation->setDuration( 250 );
                        animation->setEasingCurve( QEasingCurve::InOutQuad );
                        removeColumnsAnimationGroup->addAnimation( animation );
                    }
                }
            }

            ++floorCol;
        }
    }

    m_undoStack.endMacro();
    if (Settings::enableSounds()) {
        m_soundRemove.start();
    }

    if ( m_enableAnimation ) {
        // after finishing the animation we want to check if the game has finished
        connect(m_animation, &QSequentialAnimationGroup::finished, this, &GameScene::checkGameFinished);
        // add new animations
        m_animation->addAnimation( gravityAnimationGroup );
        m_animation->addAnimation( removeColumnsAnimationGroup );
        m_animation->start( QAbstractAnimation::KeepWhenStopped );
        // update bound lines if there are no animations
        if ( m_animation->totalDuration() == 0 ) {
            updateBoundLines();
            checkGameFinished();
        }
    }
    else {
        updateBoundLines();
        checkGameFinished();
    }
}

int GameScene::currentMarkedCount() const
{
    int marked = 0;
    for ( const Piece* p : std::as_const(m_pieces) ) {
        if ( p->m_highlighter->isVisible() ) {
            ++marked;
        }
    }
    return marked;
}

int GameScene::currentRemainCount() const
{
    int remain = 0;
    for ( const Piece* p : std::as_const(m_pieces) ) {
        if ( p->isEnabled() ) {
            ++remain;
        }
    }
    return remain;
}

void GameScene::resize( const QRectF& size )
{
    const int elementsSize1 = size.width() / PWC;
    const int elementsSize2 = size.height() / PHC;
    const int elementsSize = qMin( elementsSize1, elementsSize2 );

    // horizontal center pieces
    int gameAreaWidth = PWC * elementsSize;
    int xShift = ( size.width() - gameAreaWidth ) / 2;
    // vertical center pieces
    int gameAreaHeight = PHC * elementsSize;
    int yShift = ( size.height() - gameAreaHeight ) / 2;

    for ( int j = 0; j < PHC; ++j ) {
        for ( int i = 0; i < PWC; ++i ) {
            Piece* item = m_pieces[j*PWC+i];
            item->setRenderSize( QSize(elementsSize,elementsSize) );
            const QPoint pos( xShift + item->m_x * elementsSize, yShift + item->m_y * elementsSize );
            item->setPos( pos );
            item->m_highlighter->setRenderSize( QSize(elementsSize,elementsSize) );
            item->m_highlighter->setPos( 0, 0 );
        }
    }

    if ( m_showBoundLines ) {
        updateBoundLines();
    }

    // center the messenger
    m_messenger->setPos( size.width() / 2 - m_messenger->boundingRect().width() / 2,
                         size.height() / 2 - m_messenger->boundingRect().height() / 2 );
}

void GameScene::updateScene()
{
    resize( sceneRect() );
}

void GameScene::updateBoundLines()
{
    const int elementsSize1 = sceneRect().width() / PWC;
    const int elementsSize2 = sceneRect().height() / PHC;
    const int elementsSize = qMin( elementsSize1, elementsSize2 );

    for ( int j = 0; j < PHC; ++j ) {
        for ( int i = 0; i < PWC; ++i ) {
            Piece* item = m_pieces[j*PWC+i];
            QGraphicsLineItem* rightLine = item->m_rightLine;
            QGraphicsLineItem* bottomLine = item->m_bottomLine;
            // draw boarder if necessary, rightside and downside
            if ( !item->isEnabled() || !m_showBoundLines ) {
                rightLine->hide();
                bottomLine->hide();
                continue;
            }

            // shift one pixel, otherwise the next piece will overlap our lines
            int rightX = i + 1;
            int downY = j + 1;
            if ( rightX < PWC && m_pieces[j*PWC+rightX]->isEnabled()
                && m_pieces[j*PWC+rightX]->m_color != item->m_color ) {
                rightLine->setLine( elementsSize-1, 0-1, elementsSize-1, elementsSize-1 );
                rightLine->show();
            } else {
                rightLine->hide();
            }
            if ( downY < PHC && m_pieces[downY*PWC+i]->isEnabled()
                && m_pieces[downY*PWC+i]->m_color != item->m_color ) {
                bottomLine->setLine( 0-1, elementsSize-1, elementsSize-1, elementsSize-1 );
                bottomLine->show();
            } else {
                bottomLine->hide();
            }
        }
    }
}

void GameScene::drawBackground( QPainter* painter, const QRectF& rect )
{
    if ( Settings::radioTheme() == true ) {
        // NOTE: the following is a workaround for https://bugs.kde.org/show_bug.cgi?id=243573
        // cache the background pixmap locally in order to reduce the spritePixmap traffic when resizing
        static QByteArray theme_pre( m_renderer.theme()->identifier() );
        static QSize size_pre( rect.toRect().size() );
        static QPixmap pix( m_renderer.spritePixmap( QStringLiteral( "BACKGROUND" ), size_pre ) );
        QSize size_offset = size_pre - rect.toRect().size();
        if ( size_offset.width() < -100 || size_offset.height() < -100 || theme_pre != m_renderer.theme()->identifier() ) {
            qWarning() << "export";
            theme_pre = m_renderer.theme()->identifier();
            size_pre = rect.toRect().size() * painter->device()->devicePixelRatio();
            pix = m_renderer.spritePixmap( QStringLiteral( "BACKGROUND" ), size_pre );
            painter->drawPixmap( rect.topLeft(), pix );
        }
        else {
            painter->drawPixmap( rect.topLeft(), pix.scaled( rect.toRect().size() * painter->device()->devicePixelRatio() ) );
        }
    }
    if ( Settings::radioColor() == true ) {
        painter->fillRect( rect, Settings::bgColor() );
    }
    if ( Settings::radioImage() == true ) {
        // cache the background image locally in order to reduce the file opening traffic when resizing
        static QString img_filepath( Settings::bgImage().path() );
        static QImage img( img_filepath );
        if ( img_filepath != Settings::bgImage().path() ) {
            img_filepath = Settings::bgImage().path();
            img = QImage( img_filepath );
        }
        if ( !img.isNull() ) {
            painter->drawImage( rect, img );
        } else {
            qWarning() << "Null background image " << Settings::bgImage();
        }
    }
}

#include "moc_gamescene.cpp"

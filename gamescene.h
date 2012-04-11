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

#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QUndoStack>
#include <QVector>
#include <KGameRenderer>

class QSequentialAnimationGroup;
class KConfigGroup;
class KGamePopupItem;
class Piece;
/**
 * This class represents the game field scene.
 * It handles the most important game logic and rendering.
 */
class GameScene : public QGraphicsScene
{
    Q_OBJECT
    public:
        /** Constructor */
        explicit GameScene( QObject* parent = 0 );
        /** Destructor */
        ~GameScene();
        /** Start a new game with custom size, color count and game id */
        void startNewGame( int pwc = 10, int phc = 16, int colorCount = 5, int gameId = qrand() );
        /** Load a game from config */
        void loadGame( const KConfigGroup& config );
        /** Save the current game to config */
        void saveGame( KConfigGroup& config ) const;
        /** Restart the current game, all the undo/redo history will be lost */
        void restartGame();
        /** Pause/Unpause the current game */
        void setPaused( bool isPaused );
        /** @return whether the current game has been finished */
        bool isGameFinished() const;
        /** Change the background type */
        void setBackgroundType( int type );
        /** Determine whether to show the bound lines between different colors */
        void setShowBoundLines( bool isShowing );
        /** Determine whether to enable the animation when removing pieces */
        void setEnableAnimation( bool isEnabled );
        /** Determine whether the pieces should be highlighted when hovered */
        void setEnableHighlight( bool isEnabled );
        /** The MainWindow needs a ref. to this for KgThemeSelector */
        KgThemeProvider* themeProvider() const;
    Q_SIGNALS:
        /** Emitted when undo action enable or disable */
        void canUndoChanged( bool canUndo );
        /** Emitted when redo action enable or disable */
        void canRedoChanged( bool canRedo );
        /** Emitted when the marked piece count changes */
        void markedCountChanged( int markedCount );
        /** Emitted when the remaining piece count changes */
        void remainCountChanged( int remainCount );
        /** Emitted when the game has been finished */
        void gameFinished( int remainCount );
    public Q_SLOTS:
        /** Undo the last move */
        void undoMove();
        /** Redo the last undo */
        void redoMove();
        /** Undo all */
        void undoAllMove();
        /** Redo all */
        void redoAllMove();
    protected:
        /** Reimplemented for drawing the background depending on the settings */
        virtual void drawBackground( QPainter* painter, const QRectF& rect );
    private Q_SLOTS:
        /** Check whether the player has no way to remove any pieces, emit signals if finished */
        void checkGameFinished();
        /** Try to highlight the pieces of the same color at ( x y ) */
        void highlightPieces( int x, int y );
        /** Try to unhighlight the pieces of the same color at ( x y ) */
        void unhighlightPieces( int x, int y );
        /** Try to remove the pieces of the same color at ( x y ) */
        void removePieces( int x, int y );
        /** Resize the game scene, layout all the pieces and bound lines */
        void resize( const QRectF& size );
        /** Update all the pieces and bound lines */
        void updateScene();
        /** Update all the bound lines */
        void updateBoundLines();
    private:
        /** Internal function used to help perform piece function recursively */
        void traverseNeighbors( int x, int y, int color, bool (GameScene::*func)(Piece*) );
        /** Internal function used to help highlight pieces recursively */
        bool highlightPiece( Piece* p );
        /** Internal function used to help unhighlight pieces recursively */
        bool unhighlightPiece( Piece* p );
        /** Internal function used to help remove pieces recursively */
        bool removePiece( Piece* p );
        /** Internal function used to check whether the piece can be removed */
        bool canRemovePiece( int x, int y );
        /** Internal function used to caluculate the marked piece count */
        int currentMarkedCount() const;
        /** Internal function used to caluculate the remaining piece count */
        int currentRemainCount() const;
        /** The game graphics item renderer */
        KGameRenderer m_renderer;
        /** The popup messenger used when game paused or finished */
        KGamePopupItem* m_messenger;
        /** The container of all the pieces */
        QVector<Piece*> m_pieces;
        /** The undo stack recording moves */
        QUndoStack m_undoStack;
        /** True if the bound lines should be showed */
        bool m_showBoundLines;
        /** True if the animation of removing pieces should be used */
        bool m_enableAnimation;
        /** True if the pieces should be highlighted when hovered */
        bool m_enableHighlight;
        /** The background type */
        int m_backgroundType;
        /** The count of pieces in a row */
        int PWC;
        /** The count of pieces in a column */
        int PHC;
        /** The count of colors used */
        int m_colorCount;
        /** The game id */
        int m_gameId;
        /** True if the game has been paused */
        bool m_isPaused;
        /** True if the game has been finished */
        bool m_isFinished;
        /** The animation group holding the animations of removing pieces */
        QSequentialAnimationGroup* const m_animation;
};

#endif // GAMESCENE_H

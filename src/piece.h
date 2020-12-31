/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef PIECE_H
#define PIECE_H

#include <KGameRenderedObjectItem>

class QGraphicsLineItem;
class QGraphicsSceneMouseEvent;
/**
 * This class represents a single piece on the game scene.
 * It emits signals when clicked.
 */
class Piece : public KGameRenderedObjectItem
{
    Q_OBJECT
    public:
        /** Constructor */
        explicit Piece( KGameRenderer* renderer, int x, int y, int color, QGraphicsItem* parent = nullptr );
        /** Destructor */
        ~Piece();
        /** Reimplemented for using bounding rect for detecting hovering and mouse clicking */
        QPainterPath shape() const override;
        /** The current column in the game scene, from left to right, starts from 0 */
        int m_x;
        /** The current row in the game scene, from top to bottom, starts from 0 */
        int m_y;
        /** The color of the piece */
        const int m_color;
        /** The bound line graphics item on the right side */
        QGraphicsLineItem* const m_rightLine;
        /** The bound line graphics item on the bottom side */
        QGraphicsLineItem* const m_bottomLine;
        /** The highlight graphics item overlapped */
        KGameRenderedObjectItem* const m_highlighter;
    Q_SIGNALS:
        /** Emitted when this piece is clicked */
        void pieceClicked( int x, int y );
        /** Emitted when this piece is hovered */
        void pieceHovered( int x, int y );
        /** Emitted when this piece is unhovered */
        void pieceUnhovered( int x, int y );
    protected:
        /** Reimplemented for emitting signals if this piece is hovered */
        void hoverEnterEvent( QGraphicsSceneHoverEvent* event ) override;
        /** Reimplemented for emitting signals if this piece is unhovered */
        void hoverLeaveEvent( QGraphicsSceneHoverEvent* event ) override;
        /** Reimplemented for emitting signals if any mouse click event */
        void mousePressEvent( QGraphicsSceneMouseEvent* event ) override;
};

#endif // PIECE_H

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
        explicit Piece( KGameRenderer* renderer, int x, int y, int color, QGraphicsItem* parent = 0 );
        /** Destructor */
        ~Piece();
        /** Reimplemented for using bounding rect for detecting hovering and mouse clicking */
        virtual QPainterPath shape() const;
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
        virtual void hoverEnterEvent( QGraphicsSceneHoverEvent* event );
        /** Reimplemented for emitting signals if this piece is unhovered */
        virtual void hoverLeaveEvent( QGraphicsSceneHoverEvent* event );
        /** Reimplemented for emitting signals if any mouse click event */
        virtual void mousePressEvent( QGraphicsSceneMouseEvent* event );
};

#endif // PIECE_H

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

#ifndef UNDO_H
#define UNDO_H

#include <QPointF>
#include <QUndoCommand>

#include "piece.h"

const int ID_HIDEPIECE = 100;
const int ID_SWAPPIECE = 200;

class HidePiece : public QUndoCommand
{
    public:
        explicit HidePiece( Piece* piece ) : m_piece(piece) {
            setText( "HidePiece" );
        }
        virtual int id() const {
            return ID_HIDEPIECE;
        }
        virtual void undo() {
            m_piece->setEnabled( true );
            m_piece->show();
        }
        virtual void redo() {
            m_piece->setEnabled( false );
            m_piece->hide();
        }
    private:
        friend void GameScene::loadGame( const KConfigGroup& config );
        friend void GameScene::saveGame( KConfigGroup& config ) const;
        Piece* m_piece;
};

class SwapPiece : public QUndoCommand
{
    public:
        explicit SwapPiece( Piece** a, Piece** b, const QPointF& posA, const QPointF& posB )
        : m_pieceA(a), m_pieceB(b), m_posA(posA), m_posB(posB) {
            setText( "SwapPiece" );
        }
        virtual int id() const {
            return ID_SWAPPIECE;
        }
        virtual void undo() {
            redo();
        }
        virtual void redo() {
            Piece* tmpA = *m_pieceA;
            Piece* tmpB = *m_pieceB;
            int tmpAX = tmpA->m_x;
            int tmpAY = tmpA->m_y;
            int tmpBX = tmpB->m_x;
            int tmpBY = tmpB->m_y;
            *m_pieceA = tmpB;
            (*m_pieceA)->m_x = tmpAX;
            (*m_pieceA)->m_y = tmpAY;
            *m_pieceB = tmpA;
            (*m_pieceB)->m_x = tmpBX;
            (*m_pieceB)->m_y = tmpBY;
            (*m_pieceA)->setPos( m_posA );
            (*m_pieceB)->setPos( m_posB );
        }
    private:
        Piece** m_pieceA;
        Piece** m_pieceB;
        const QPointF m_posA;
        const QPointF m_posB;
};

#endif // UNDO_H

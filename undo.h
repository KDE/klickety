/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
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
            setText( QStringLiteral( "Hide Piece" ) );
        }
        int id() const Q_DECL_OVERRIDE {
            return ID_HIDEPIECE;
        }
        void undo() Q_DECL_OVERRIDE {
            m_piece->setEnabled( true );
            m_piece->show();
        }
        void redo() Q_DECL_OVERRIDE {
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
            setText( QStringLiteral( "Swap Piece" ) );
        }
        int id() const Q_DECL_OVERRIDE {
            return ID_SWAPPIECE;
        }
        void undo() Q_DECL_OVERRIDE {
            redo();
        }
        void redo() Q_DECL_OVERRIDE {
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

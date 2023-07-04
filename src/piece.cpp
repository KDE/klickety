/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "piece.h"

#include "gamescene.h"

#include <QGraphicsLineItem>

Piece::Piece( KGameRenderer* renderer, int x, int y, int color, QGraphicsItem* parent )
: KGameRenderedObjectItem(renderer,QStringLiteral( "BLOCK_%1" ).arg(QString::number(color)),parent),
m_x(x),
m_y(y),
m_color(color),
m_rightLine(new QGraphicsLineItem),
m_bottomLine(new QGraphicsLineItem),
m_highlighter(new KGameRenderedObjectItem(renderer,QStringLiteral( "HIGHLIGHT" )))
{
    setAcceptHoverEvents( true );
    setAcceptedMouseButtons( Qt::LeftButton );
    m_highlighter->setAcceptHoverEvents( false );
    m_highlighter->setAcceptedMouseButtons( Qt::NoButton );
}

Piece::~Piece()
{
    delete m_rightLine;
    delete m_bottomLine;
    delete m_highlighter;
}

QPainterPath Piece::shape() const
{
    QPainterPath path;
    path.addRect( boundingRect() );
    return path;
}

void Piece::hoverEnterEvent( QGraphicsSceneHoverEvent* event )
{
    Q_UNUSED(event)
    Q_EMIT pieceHovered( m_x, m_y );
}

void Piece::hoverLeaveEvent( QGraphicsSceneHoverEvent* event )
{
    Q_UNUSED(event)
    Q_EMIT pieceUnhovered( m_x, m_y );
}

void Piece::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    Q_UNUSED(event)
    Q_EMIT pieceClicked( m_x, m_y );
}

#include "moc_piece.cpp"

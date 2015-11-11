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
    m_highlighter->setAcceptedMouseButtons( 0 );
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
    emit pieceHovered( m_x, m_y );
}

void Piece::hoverLeaveEvent( QGraphicsSceneHoverEvent* event )
{
    Q_UNUSED(event)
    emit pieceUnhovered( m_x, m_y );
}

void Piece::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    Q_UNUSED(event)
    emit pieceClicked( m_x, m_y );
}

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

#include <QGraphicsLineItem>

#include "gamescene.h"
#include "piece.h"

Piece::Piece( KGameRenderer* renderer, int x, int y, int color, QGraphicsItem* parent )
: KGameRenderedObjectItem(renderer,QString("BLOCK_%1").arg(QString::number(color)),parent),
m_x(x),
m_y(y),
m_color(color),
m_rightLine(new QGraphicsLineItem),
m_bottomLine(new QGraphicsLineItem)
{
    setAcceptedMouseButtons( Qt::LeftButton );
}

Piece::~Piece()
{
    delete m_rightLine;
    delete m_bottomLine;
}

void Piece::mousePressEvent( QGraphicsSceneMouseEvent* event )
{
    Q_UNUSED(event)
    emit pieceClicked( m_x, m_y );
}

/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "gameview.h"

GameView::GameView( QGraphicsScene* scene, QWidget* parent )
: QGraphicsView(scene,parent)
{
}

void GameView::resizeEvent( QResizeEvent *event )
{
    Q_UNUSED(event)
    scene()->setSceneRect( rect() );
    setTransform( QTransform() );
}

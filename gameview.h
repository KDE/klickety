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

#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>

class QGraphicsScene;
/**
 * This class represents a graphics view for the game scene.
 * It is just a standard QGraphicsView but specialized for resizeEvent.
 * When resizing, it can notify the scene so that the scene can adjust
 * its layout according to the new size.
 */
class GameView : public QGraphicsView
{
    public:
        /** Constructor */
        explicit GameView( QGraphicsScene* scene, QWidget* parent = nullptr );
    protected:
        /** Reimplemented for notifying game scene of the resize event */
        void resizeEvent( QResizeEvent *event ) Q_DECL_OVERRIDE;
};

#endif // GAMEVIEW_H

/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
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
        void resizeEvent( QResizeEvent *event ) override;
};

#endif // GAMEVIEW_H

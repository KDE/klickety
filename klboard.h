/***************************************************************************
 *   Copyright (c) 1995, Eirik Eng                                         *
 *                                                                         *
 *   Copyright (c) 1996-2004, Nicolas Hadacek                              *
 *   hadacek@kde.org                                                       *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA               *
 ***************************************************************************/

#ifndef KL_BOARD_H
#define KL_BOARD_H



#include <QVector>


#include "base/baseboard.h"
#include "base/piece.h"



class KlBoard : public BaseBoard
{
 Q_OBJECT
	public:
		KlBoard(QWidget *parent);

    void start(const GTInitData &data);


	signals:
		/**
		* @brief Signal the game start
		* Used by the LCD clock.
		*/
		void firstBlockClicked();


 private:
    KGrid2D::Square<int> field;
    bool                sliding;
    QVector<bool>     empty;
    uint                addRemoved;
    bool                blocked;

    KGrid2D::Coord findSprite(Sprite *) const;
    AfterRemoveResult afterRemove(bool doAll, bool first);
    bool afterAfterRemove();
    bool toBeRemoved(const KGrid2D::Coord &) const;
    void remove();
    bool toFall(const KGrid2D::Coord &) const;
    bool toSlide(const KGrid2D::Coord &) const;
    bool doSlide(bool doAll, bool first, bool lineByLine);
    void computeInfos();

    void mouseReleaseEvent(QMouseEvent* e);
};

#endif //KL_BOARD_H

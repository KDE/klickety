/***************************************************************************
 *   Copyright (c) 1995, Eirik Eng                                         *
 *   hadacek@kde.org                                                       *
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

#include "klboard.h"



#include <QMouseEvent>
#include <QVector>


#include <kdebug.h>


#include "base/factory.h"



using namespace KGrid2D;

//
// Constructor / Destructor
//

KlBoard::KlBoard(QWidget *parent)
    : BaseBoard(true, parent),
      field(blocksMatrix().width(), blocksMatrix().height()),
      empty(blocksMatrix().width()),
      blocked(false)
{
}



//
// Public & Private
//

void KlBoard::mouseReleaseEvent(QMouseEvent* e)
{
	if (e->button()==Qt::LeftButton && !blocked) {
		QList<QGraphicsItem*> list = scene()->items(mapToScene(e->pos()));
		if ( list.count()!=0 ) {
			Sprite *spr = static_cast<Sprite *>(list.first());
			field.fill(0);
			addRemoved = findGroup(field, findSprite(spr));
			if ( addRemoved>=2 ) {
				kDebug() << "Click\n";
				if ( state!=Normal ) {
					state = Normal;
					emit firstBlockClicked();
				}
				blocked = true;
				_beforeRemove(true);
			}
		}
	}
}

void KlBoard::start(const GTInitData &data)
{
    BaseBoard::start(data);

    updateScore(blocksMatrix().width() * blocksMatrix().height());
    state = GameOver;
    sliding = false;
    blocked = false;
    for (uint i=0; i<blocksMatrix().width(); i++)
        for (uint j=0; j<blocksMatrix().height(); j++) {
            Block *block = new Block();
            block->setValue(Piece::info().generateType(&randomSequence()), main);
            Coord c(i, j);
            setBlock(c, block);
        }
    computeInfos();
    showBoard(true);
}

Coord KlBoard::findSprite(Sprite *spr) const
{
    for (uint i=0; i<blocksMatrix().width(); i++)
        for (uint j=0; j<blocksMatrix().height(); j++) {
            Coord c(i, j);
            if ( blocksMatrix()[c] && blocksMatrix()[c]->sprite()==spr ) return c;
        }
    Q_ASSERT(false);
    return Coord();
}

bool KlBoard::toBeRemoved(const Coord &c) const
{
    return ( field[c]==-1 );
}

void KlBoard::remove()
{
	kDebug() << "1\n";
    BaseBoard::remove();
	kDebug() << "2\n";
    updateRemoved(nbRemoved() + addRemoved);
    updateScore(score() - addRemoved);
}

bool KlBoard::toFall(const Coord &c) const
{
    Coord under(c.first, c.second-1);
    return ( blocksMatrix()[under]==0 );
}

void KlBoard::computeInfos()
{
    BaseBoard::computeInfos();
    if ( graphic() ) computeNeighbours();
    empty.fill(true);
    for (uint i=0; i<blocksMatrix().width(); i++)
        for (uint j=0; j<firstClearLine(); j++) {
            Coord c(i, j);
            if ( blocksMatrix()[c]!=0 ) empty[i] = false;
        }
}

bool KlBoard::toSlide(const Coord &c) const
{
    return empty[c.first-1];
}

bool KlBoard::doSlide(bool doAll, bool first, bool lineByLine)
{
	Q_ASSERT( !lineByLine || !doAll );
	
	if ( !doAll ) {
		if (first)
			loop = 0;
		else 
			loop++;
	}
	bool final = (doAll || lineByLine || loop==bfactory->bbi.nbFallStages);

    for (uint j=0; j<firstClearLine(); j++) {
        // compute
        uint h = 0;
        QVector<uint> heights(blocksMatrix().width());
        for (uint i=1; i<blocksMatrix().width(); i++) { // first column cannot slide
            Coord src(i, j);
            if ( toSlide(src) ) h++;
            heights[i] = h;
        }

        // do move
        for (uint i=1; i<blocksMatrix().width(); i++) {
            Coord src(i, j);
            if( heights[i]==0 || blocksMatrix()[src]==0 ) continue;
            if (lineByLine) final = false;
            uint k = i - (lineByLine ? 1 : heights[i]);
            Coord dest(k, j);
            if ( final || lineByLine ) moveBlock(src, dest);
            else partialBlockFall(src, dest);
        }
    }

    main->update();
    if (final) computeInfos();

    return final;
}

BaseBoard::AfterRemoveResult KlBoard::afterRemove(bool doAll, bool first)
{
    AfterRemoveResult res = Done; // dummy default
    if (sliding) {
        res = (doSlide(doAll, loop==bfactory->bbi.nbFallStages+1, false) ? Done
           : NeedAfterRemove);
        if ( res==Done ) sliding = false;
    } else {
        res = BaseBoard::afterRemove(doAll, first);
        if ( res==Done ) {
            res = NeedAfterRemove;
            sliding = true;
            loop++;
        }
    }
    return res;
}

bool KlBoard::afterAfterRemove()
{
    // check if there are remaining groups
    field.fill(0);
    QVector<uint> groups = findGroups(field, 2, true);
    blocked = false;
    return groups.size()!=0;
}

#include "klboard.moc"

#include "board.h"
#include "board.moc"

#include "base/factory.h"


using namespace KGrid2D;

void KLBoard::contentsMouseReleaseEvent(QMouseEvent *e)
{
    if ( e->button()!=LeftButton || blocked ) return;
    QCanvasItemList list = canvas()->collisions(e->pos());
    if ( list.count()==0 ) return;

    QCanvasSprite *spr = static_cast<QCanvasSprite *>(list.first());
    Coord c = findSprite(spr);
    field.fill(0);
    addRemoved = findGroup(field, c);
    if ( addRemoved>=2 ) {
        if ( state!=Normal ) {
            state = Normal;
            emit firstBlockClicked();
        }
        blocked = true;
        _beforeRemove(true);
    }
}

KLBoard::KLBoard(QWidget *parent)
    : BaseBoard(true, parent),
      field(matrix().width(), matrix().height()),
      empty(matrix().width()),
      blocked(false)
{}

void KLBoard::start(const GTInitData &data)
{
    BaseBoard::start(data);

    updateScore(matrix().width() * matrix().height());
    state = GameOver;
    sliding = false;
    blocked = false;
    for (uint i=0; i<matrix().width(); i++)
        for (uint j=0; j<matrix().height(); j++) {
            Block *block = new Block;
            block->setValue(Piece::info().generateType(&randomSequence()), main);
            Coord c(i, j);
            setBlock(c, block);
        }
    computeInfos();
    showBoard(true);
}

Coord KLBoard::findSprite(QCanvasSprite *spr) const
{
    for (uint i=0; i<matrix().width(); i++)
        for (uint j=0; j<matrix().height(); j++) {
            Coord c(i, j);
            if ( matrix()[c] && matrix()[c]->sprite()==spr ) return c;
        }
    Q_ASSERT(false);
    return Coord();
}

bool KLBoard::toBeRemoved(const Coord &c) const
{
    return ( field[c]==-1 );
}

void KLBoard::remove()
{
    BaseBoard::remove();
    updateRemoved(nbRemoved() + addRemoved);
    updateScore(score() - addRemoved);
}

bool KLBoard::toFall(const Coord &c) const
{
    Coord under(c.first, c.second-1);
    return ( matrix()[under]==0 );
}

void KLBoard::computeInfos()
{
    BaseBoard::computeInfos();
    if ( graphic() ) computeNeighbours();
    empty.fill(true);
    for (uint i=0; i<matrix().width(); i++)
        for (uint j=0; j<firstClearLine(); j++) {
            Coord c(i, j);
            if ( matrix()[c]!=0 ) empty[i] = false;
        }
}

bool KLBoard::toSlide(const Coord &c) const
{
    return empty[c.first-1];
}

bool KLBoard::doSlide(bool doAll, bool first, bool lineByLine)
{
    Q_ASSERT( !lineByLine || !doAll );

    if ( !doAll ) {
		if (first) loop = 0;
		else loop++;
    }
	bool final = (doAll || lineByLine || loop==bfactory->bbi.nbFallStages);

    for (uint j=0; j<firstClearLine(); j++) {
        // compute
        uint h = 0;
        QMemArray<uint> heights(matrix().width());
        for (uint i=1; i<matrix().width(); i++) { // first column cannot slide
            Coord src(i, j);
            if ( toSlide(src) ) h++;
            heights[i] = h;
        }

        // do move
        for (uint i=1; i<matrix().width(); i++) {
            Coord src(i, j);
            if( heights[i]==0 || matrix()[src]==0 ) continue;
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

BaseBoard::AfterRemoveResult KLBoard::afterRemove(bool doAll, bool first)
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

bool KLBoard::afterAfterRemove()
{
    // check if there are remaining groups
    field.fill(0);
    QMemArray<uint> groups = findGroups(field, 2, true);
    blocked = false;
    return groups.size()!=0;
}

#ifndef KL_BOARD_H
#define KL_BOARD_H

#include "base/board.h"
#include "base/piece.h"


class KLBoard : public BaseBoard
{
 Q_OBJECT
 public:
	KLBoard(QWidget *parent);

    void start();

 signals:
    void firstBlockClicked();

 private:
    Grid2D::Square<int> field;
    bool                sliding;
    QMemArray<bool>     empty;
    uint                addRemoved;
    bool                blocked;

    Grid2D::Coord findSprite(QCanvasSprite *) const;
    AfterRemoveResult afterRemove(bool doAll, bool first);
    void afterAfterRemove();
    bool toBeRemoved(const Grid2D::Coord &) const;
    void remove();
    bool toFall(const Grid2D::Coord &) const;
    bool toSlide(const Grid2D::Coord &) const;
    bool doSlide(bool doAll, bool first, bool lineByLine);
    void computeInfos();

    void contentsMouseReleaseEvent(QMouseEvent *);
};

#endif

#ifndef KL_BOARD_H
#define KL_BOARD_H

#include "base/board.h"
#include "base/piece.h"
//Added by qt3to4:
#include <QMouseEvent>
#include <QVector>

class KLBoard : public BaseBoard
{
 Q_OBJECT
 public:
	KLBoard(QWidget *parent);

    void start(const GTInitData &data);

 signals:
    void firstBlockClicked();

 private:
    KGrid2D::Square<int> field;
    bool                sliding;
    QVector<bool>     empty;
    uint                addRemoved;
    bool                blocked;

    KGrid2D::Coord findSprite(Q3CanvasSprite *) const;
    AfterRemoveResult afterRemove(bool doAll, bool first);
    bool afterAfterRemove();
    bool toBeRemoved(const KGrid2D::Coord &) const;
    void remove();
    bool toFall(const KGrid2D::Coord &) const;
    bool toSlide(const KGrid2D::Coord &) const;
    bool doSlide(bool doAll, bool first, bool lineByLine);
    void computeInfos();

    void contentsMouseReleaseEvent(QMouseEvent *);
};

#endif

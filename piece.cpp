#include "piece.h"

#include <qpainter.h>

#include "base/board.h"


const QColor KLPieceInfo::COLORS[NB_BLOCK_TYPES] = {
    QColor(200, 100, 100), // red
	QColor(100, 200, 100), // green
	QColor(100, 100, 200), // blue
	QColor(200, 200, 100), // orange
    QColor(200, 100, 200)  // violet
};

void KLPieceInfo::draw(QPixmap *pixmap, uint blockType, uint bMode,
                       bool lighted) const
{
	QColor col = COLORS[blockType];
    if (lighted) col = col.light();
    pixmap->fill(col);

	QPainter p(pixmap);
    QRect r = pixmap->rect();

    p.setPen(col.dark());
    if ( !(bMode & BaseBoard::Up) )
        p.drawLine(r.topLeft(), r.topRight());
    if ( !(bMode & BaseBoard::Down) )
        p.drawLine(r.bottomLeft(), r.bottomRight());
    if ( !(bMode & BaseBoard::Left) )
        p.drawLine(r.topLeft(), r.bottomLeft());
    if ( !(bMode & BaseBoard::Right) )
        p.drawLine(r.topRight(),r.bottomRight());

    p.setPen(col.dark(110));
    if (bMode & BaseBoard::Up)
        p.drawLine(r.topLeft()+QPoint(1,0), r.topRight()+QPoint(-1,0));
    if (bMode & BaseBoard::Down)
        p.drawLine(r.bottomLeft()+QPoint(1,0), r.bottomRight()+QPoint(-1,0));
    if (bMode & BaseBoard::Left)
        p.drawLine(r.topLeft()+QPoint(0,1), r.bottomLeft()+QPoint(0,-1));
    if (bMode & BaseBoard::Right)
        p.drawLine(r.topRight()+QPoint(0,1), r.bottomRight()+QPoint(0,-1));
}

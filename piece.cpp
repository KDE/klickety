#include "piece.h"

#include <qpainter.h>
#include <klocale.h>
#include "base/board.h"


const char *KLPieceInfo::DEFAULT_COLORS[NB_BLOCK_TYPES] = {
    "#C86464", "#64C864", "#6464C8", "#C8C864", "#C864C8"
};

QColor KLPieceInfo::defaultColor(uint i) const
{
  if ( i>=nbColors() ) return QColor();
  return QColor(DEFAULT_COLORS[i]);
}

QString KLPieceInfo::colorLabel(uint i) const
{
    return i18n("Color #%1:").arg(i+1);
}

void KLPieceInfo::draw(QPixmap *pixmap, uint blockType, uint bMode,
                       bool lighted) const
{
	QColor col = color(blockType);
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

#include "highscores.h"

#include <klocale.h>

#include "base/factory.h"
#include "base/board.h"


using namespace KExtHighscores;

KLHighscores::KLHighscores()
{
    uint worstScore = bfactory->bbi.width * bfactory->bbi.height + 1;
    Item *item = new ScoreItem(worstScore);
    setItem("score", item);

    item = new BestScoreItem(worstScore);
    setItem("best score", item);

    item = new Item((uint)0, i18n("Elapsed time"), Qt::AlignRight);
    item->setPrettyFormat(Item::MinuteTime);
    setItem("time", item);
}

bool KLHighscores::isStrictlyLess(const Score &s1, const Score &s2) const
{
    if ( s1.score()==s2.score() )
        return s1.data("time").toUInt()<s2.data("time").toUInt();
    return s1.score()>s2.score();
}

void KLHighscores::additionnalQueryItems(KURL &url, const Score &s) const
{
    uint time = s.data("time").toUInt();
    addToQueryURL(url, "scoreTime", QString::number(time));
}

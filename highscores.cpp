#include "highscores.h"

#include <klocale.h>

#include "base/factory.h"
#include "base/board.h"


namespace KExtHighscores
{

ExtHighscores::ExtHighscores()
    : Highscores(bfactory->mainData.version, 0/*bfactory->mainData.homepage*/)
{
    ScoreItem *sitem =
        new ScoreItem(bfactory->bbi.width * bfactory->bbi.height);
    setItem("score", sitem);

    Item *item = new Item((uint)0, i18n("Elapsed time"), Qt::AlignRight);
    item->setPrettyFormat(Item::MinuteTime);
    setItem("time", item);
}

bool ExtHighscores::isStrictlyLess(const Score &s1, const Score &s2) const
{
    if ( s1.score()==s2.score() )
        return s1.data("time").toUInt()<s2.data("time").toUInt();
    return s1.score()>s2.score();
}

void ExtHighscores::additionnalQueryItems(KURL &url, const Score &s) const
{
    uint time = s.data("time").toUInt();
    addToQueryURL(url, "scoreTime", QString::number(time));
}

};

#include "highscores.h"

#include "base/factory.h"
#include "base/board.h"


using namespace KExtHighscore;

KLHighscores::KLHighscores()
{
    Item *item = createItem(ScoreDefault);
    setScoreItem(bfactory->bbi.width * bfactory->bbi.height + 1, item);
    addScoreItem("time", createItem(ElapsedTime));
}

bool KLHighscores::isStrictlyLess(const Score &s1, const Score &s2) const
{
    if ( s1.score()==s2.score() )
        return s1.data("time").toUInt()<s2.data("time").toUInt();
    return s1.score()>s2.score();
}

void KLHighscores::additionalQueryItems(KURL &url, const Score &s) const
{
    uint time = s.data("time").toUInt();
    addToQueryURL(url, "scoreTime", QString::number(time));
}

#ifndef KL_HIGHSCORES_H
#define KL_HIGHSCORES_H

#include "base/highscores.h"


class KLHighscores : public BaseHighscores
{
 public:
    KLHighscores();

 private:
    bool isStrictlyLess(const KExtHighscore::Score &,
                        const KExtHighscore::Score &) const;
    void additionalQueryItems(KURL &url, const KExtHighscore::Score &) const;
};

#endif

#ifndef KL_HIGHSCORES_H
#define KL_HIGHSCORES_H

#include "base/highscores.h"


class KLHighscores : public BaseHighscores
{
 public:
    KLHighscores();

 private:
    bool isStrictlyLess(const KExtHighscores::Score &,
                        const KExtHighscores::Score &) const;
    void additionnalQueryItems(KURL &url, const KExtHighscores::Score &) const;
};

#endif

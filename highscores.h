#ifndef KL_HIGHSCORES_H
#define KL_HIGHSCORES_H

#include "ghighscores.h"

#include "base/factory.h"


namespace KExtHighscores
{

class ExtHighscores : public Highscores
{
 public:
    ExtHighscores(const MainData &, const BaseBoardInfo &);

 private:
    bool isStrictlyLess(const Score &, const Score &) const;
    void additionnalQueryItems(KURL &url, const Score &) const;
};

};

#endif

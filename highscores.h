#ifndef KL_HIGHSCORES_H
#define KL_HIGHSCORES_H

#include "generic/ghighscores.h"


namespace KExtHighscores
{

class ExtHighscores : public Highscores
{
 public:
    ExtHighscores();

 private:
    bool isStrictlyLess(const Score &, const Score &) const;
    void additionnalQueryItems(KURL &url, const Score &) const;
};

};

#endif

#ifndef KL_MAIN_H
#define KL_MAIN_H

#include "base/main.h"
#include "base/factory.h"


class KLFactory : public BaseFactory
{
 public:
    KLFactory();

 protected:
    BaseBoard *createBoard(bool graphic, QWidget *parent);
    BaseInterface *createInterface(QWidget *parent);

    KConfigWidget *createGameConfig() { return 0; }
};

class KLMainWindow : public BaseMainWindow
{
 Q_OBJECT
 public:
    KLMainWindow();
};

#endif

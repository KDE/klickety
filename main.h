#ifndef KL_MAIN_H
#define KL_MAIN_H

#include "base/main.h"
#include "base/factory.h"


class KLMainWindow : public BaseMainWindow, public BaseFactory
{
 Q_OBJECT
 public:
    KLMainWindow();

 protected:
    BaseBoard *_createBoard(bool graphic, QWidget *parent);
    BaseField *_createField(QWidget *) { return 0; }

    KConfigWidget *_createAppearanceConfig();
    KConfigWidget *_createGameConfig() { return 0; }
};

#endif

#ifndef KL_MAIN_H
#define KL_MAIN_H

#include "base/main.h"
#include "base/factory.h"
#include "board.h"
#include "field.h"


class KLFactory : public BaseFactory
{
 public:
    KLFactory();

 protected:
    virtual BaseBoard *createBoard(bool, QWidget *parent)
        { return new KLBoard(parent); }
    virtual BaseInterface *createInterface(QWidget *parent)
        { return new Field(parent); }
};

class KLMainWindow : public BaseMainWindow
{
 Q_OBJECT
 public:
    KLMainWindow();
};

#endif

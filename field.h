#ifndef KL_FIELD_H
#define KL_FIELD_H

#include <qwidget.h>

#include "base/field.h"
#include "base/inter.h"

class KGameLCDClock;

class Field : public QWidget, public BaseField, public BaseInterface
{
 Q_OBJECT
 public:
    Field(QWidget *parent);
  
 private slots:
    void scoreUpdatedSlot() { scoreUpdated(); }
    void start();
    void gameOver();
    void settingsChanged() { BaseField::settingsChanged(); }

 private:
    KGameLCDClock *elapsedTime;

    void pause();
    KExtHighscore::Score currentScore() const;
    void _start() { start(); }
    void _pause() { pause(); }
    bool _isPaused() const;
};

#endif

#include "field.h"
#include "field.moc"

#include <qwhatsthis.h>
#include <qlayout.h>

#include <kapplication.h>
#include <klocale.h>
#include <kgamelcd.h>

#include "base/board.h"


Field::Field(QWidget *parent)
    : QWidget(parent, "field"), BaseField(this)
{
    KGameLCDList *sc  = new KGameLCDList(i18n("Remaining blocks"), this);
    showScore = new KGameLCD(3, sc);
    sc->append(showScore);
    QWhatsThis::add(sc, i18n("<qt>Display the number of remaining "
                             "blocks.<br/>"
                             "It turns <font color=\"blue\">blue"
                             "</font> if it is a highscore "
                             "and <font color=\"red\">red</font> "
                             "if it is the best local score.</qt>"));
    lcds->addWidget(sc, 1, 0);
    lcds->setRowStretch(2, 1);

    KGameLCDList *et = new KGameLCDList(i18n("Elapsed time"), this);
    elapsedTime = new KGameLCDClock(et);
    connect(board, SIGNAL(firstBlockClicked()), elapsedTime, SLOT(start()));
    et->append(elapsedTime);
    lcds->addWidget(et, 5, 0);
    lcds->setRowStretch(6, 1);

    connect(board, SIGNAL(scoreUpdated()), SLOT(scoreUpdatedSlot()));
    connect(board, SIGNAL(gameOverSignal()), SLOT(gameOver()));

    settingsChanged();
    connect(parent, SIGNAL(settingsChanged()), SLOT(settingsChanged()));
    QTimer::singleShot(0, this, SLOT(start()));
}

void Field::pause()
{
    if ( board->isGameOver() ) return;
    bool paused = board->isPaused();
    if (paused) elapsedTime->start();
    else elapsedTime->stop();
    BaseField::pause(!paused);
}

void Field::start()
{
    init(false, false, true, true, QString::null);
    GTInitData data;
    data.seed = kapp->random();
    BaseField::start(data);
    elapsedTime->reset();
}

void Field::gameOver()
{
    elapsedTime->stop();
    stop(true);
    BaseField::gameOver(currentScore(), this);
}

KExtHighscore::Score Field::currentScore() const
{
    KExtHighscore::Score score(KExtHighscore::Won);
    score.setScore(board->score());
    score.setData("time", 3600 - elapsedTime->seconds());
    return score;
}

bool Field::_isPaused() const
{
    return board->isPaused();
}

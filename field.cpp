#include "field.h"
#include "field.moc"

#include <qwhatsthis.h>
#include <qlayout.h>

#include <kapplication.h>
#include <klocale.h>

#include "gmisc_ui.h"
#include "base/board.h"


Field::Field(QWidget *parent)
    : QWidget(parent, "field"), BaseField(this)
{
    LCDList *sc  = new LCDList(i18n("Remaining blocks"), this);
    showScore = new LCD(3, sc);
    sc->append(showScore);
    QWhatsThis::add(sc, i18n("<qt>Display the number of remaining"
                             "blocks.<br/>"
                             "It turns <font color=\"blue\">blue"
                             "</font> if it is a highscore "
                             "and <font color=\"red\">red</font> "
                             "if it is the best local score.</qt>"));
    lcds->addWidget(sc, 1, 0);
    lcds->setRowStretch(2, 1);

    LCDList *et = new LCDList(i18n("Elapsed time"), this);
    elapsedTime = new LCDClock(et);
    connect(board, SIGNAL(firstBlockClicked()), elapsedTime, SLOT(start()));
    et->append(elapsedTime);
    lcds->addWidget(et, 5, 0);
    lcds->setRowStretch(6, 1);

    connect(board, SIGNAL(scoreUpdated(int)), SLOT(scoreUpdatedSlot(int)));
    connect(board, SIGNAL(gameOverSignal()), SLOT(gameOver()));

    settingsChanged();
    connect(parent, SIGNAL(settingsChanged()), this, SLOT(settingsChanged()));
    QTimer::singleShot(0, this, SLOT(start()));
}

QSizePolicy Field::sizePolicy() const
{
	return QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
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
    GTInitData data;
    data.seed = kapp->random();
    BaseField::init(data);
    elapsedTime->reset();
}

void Field::gameOver()
{
    elapsedTime->stop();
    stop(true);
    KExtHighscores::submitScore(currentScore(), this);
}

KExtHighscores::Score Field::currentScore() const
{
    KExtHighscores::Score score(KExtHighscores::Won);
    score.setData("score", board->score());
    score.setData("time", elapsedTime->time());
    return score;
}

bool Field::_isPaused() const
{
    return board->isPaused();
}

#include "main.h"
#include "main.moc"

#include <klocale.h>
#include <kapplication.h>
#include <khighscore.h>
#include <kaboutdata.h>

#include "base/settings.h"
#include "piece.h"
#include "highscores.h"

//-----------------------------------------------------------------------------
const MainData MAIN_DATA = {
    "klickety",
    I18N_NOOP("Klickety"),
    I18N_NOOP("Klickety is an adaptation of the \"clickomania\" game"),
    "http://klickety.sourceforge.net/",
    I18N_NOOP("Removed blocks"),
    "1.0.3",
    "1.0.3 (5 August 2004)"
};

const uint HISTOGRAM_SIZE = 16;
const uint HISTOGRAM[HISTOGRAM_SIZE] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 16, 20, 160
};

const BaseBoardInfo BASE_BOARD_INFO = {
    10, 16, false, // width, height, with pieces

    50,  // before remove time
    10,  // after removed time
    3,   // nb toggles
    7,   // nb partial fall stages

    0,   // nb arcade stages

    HISTOGRAM, HISTOGRAM_SIZE, true, // score is bound
};

KLFactory::KLFactory()
    : BaseFactory(MAIN_DATA, BASE_BOARD_INFO)
{
  _aboutData->addCredit("Dan Hill", I18N_NOOP("Icons"));
}


//-----------------------------------------------------------------------------
KLMainWindow::KLMainWindow()
{
    Field *field = static_cast<Field *>(_inter);
    buildGUI(field);
}

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    KHighscore::init(MAIN_DATA.appName);
    KLFactory flf;
    flf.init(argc, argv);

    KLPieceInfo pieceInfo;
    KLHighscores highscores;

    if ( kapp->isRestored() ) RESTORE(KLMainWindow)
    else {
        KLMainWindow *mw = new KLMainWindow;
        kapp->setMainWidget(mw);
        mw->show();
    }
	return kapp->exec();
}

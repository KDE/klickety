#include "main.h"
#include "main.moc"

#include <klocale.h>
#include <kapplication.h>

#include "base/settings.h"
#include "board.h"
#include "piece.h"
#include "highscores.h"
#include "field.h"


//-----------------------------------------------------------------------------
const MainData MAIN_DATA = {
    "klickety",
    I18N_NOOP("Klickety"),
    I18N_NOOP("Klickety is an adaptation of the \"clickomania\" game (it is "
              "similar to the \"same\" game)."),
    "http://klickety.sourceforge.net/klickety/",
    I18N_NOOP("Removed blocks"),
    "0.0.2",
    "0.0.2 (6 June 2002)"
};

const BaseBoardInfo BASE_BOARD_INFO = {
    10, 16, false, // width, height, with pieces
    false, // block size must be odd

    50,  // before remove time
    10,  // after removed time
    3,   // nb toggles
    7,   // nb partial fall stages

    0    // nb arcade stages
};

KLFactory::KLFactory()
    : BaseFactory(MAIN_DATA, BASE_BOARD_INFO)
{}

BaseBoard *KLFactory::createBoard(bool, QWidget *parent)
{
    return new KLBoard(parent);
}

BaseInterface *KLFactory::createInterface(QWidget *parent)
{
    return new Field(parent);
}


//-----------------------------------------------------------------------------
KLMainWindow::KLMainWindow()
{
    buildGUI(static_cast<Field *>(_inter));
}


//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    KLFactory flf;
    flf.init(argc, argv);

    KLPieceInfo pieceInfo;
    KExtHighscores::ExtHighscores highscores;

    if ( kapp->isRestored() ) RESTORE(KLMainWindow)
    else {
        KLMainWindow *mw = new KLMainWindow;
        kapp->setMainWidget(mw);
        mw->show();
    }
	return kapp->exec();
}

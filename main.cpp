#include "main.h"
#include "main.moc"

#include <klocale.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kapplication.h>

#include "base/settings.h"
#include "version.h"
#include "board.h"
#include "piece.h"
#include "highscores.h"
#include "field.h"


const MainData MAIN_DATA = {
    "klickety",
    I18N_NOOP("Klickety"),
    I18N_NOOP("Klickety is an adaptation of the clickomania game (it is "
              "similar to the \"same\" game)."),
    0, //"http://klickety.sourceforge.net/klickety/",
    I18N_NOOP("Removed blocks")
};

const BaseBoardInfo BASE_BOARD_INFO = {
    10, 16, false, // width, height, with pieces
    false, // block size must be odd

    50,  // before remove time
    10,  // after removed time
    3,   // nb toggles
    7,   // nb partial fall stages
};

BaseBoard *KLMainWindow::_createBoard(bool, QWidget *parent)
{
    return new KLBoard(parent);
}

KLMainWindow::KLMainWindow()
    : BaseFactory(&MAIN_DATA, &BASE_BOARD_INFO)
{
    Field *field = new Field(this);
    _inter = field;
    buildGUI(field);
}

KConfigWidget *KLMainWindow::_createAppearanceConfig()
{
    return new AppearanceConfig;
}


//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    KLocale::setMainCatalogue("ksirtet");
	KAboutData aboutData(MAIN_DATA.name, MAIN_DATA.trName, LONG_VERSION,
                         MAIN_DATA.description, KAboutData::License_GPL,
                         COPYLEFT, 0, MAIN_DATA.homepage);
    aboutData.addAuthor("Nicolas Hadacek", 0, "hadacek@kde.org");
	aboutData.addAuthor("Eirik Eng", I18N_NOOP("Core engine"));
	KCmdLineArgs::init(argc, argv, &aboutData);

	KApplication a;
    KGlobal::locale()->insertCatalogue("libkdegames");
    KGlobal::locale()->insertCatalogue("libkdehighscores");

    KLPieceInfo pieceInfo;
    KExtHighscores::ExtHighscores highscores(MAIN_DATA, BASE_BOARD_INFO);

    if ( kapp->isRestored() ) RESTORE(KLMainWindow)
    else {
        KLMainWindow *mw = new KLMainWindow;
        kapp->setMainWidget(mw);
        mw->show();
    }
	return kapp->exec();
}

#include "main.h"
#include "main.moc"


#include <klocale.h>
#include <kapplication.h>
#include <khighscore.h>


#include "base/settings.h"
#include "field.h"
#include "piece.h"
#include "highscores.h"
#include "klfactory.h"



//-----------------------------------------------------------------------------
KLMainWindow::KLMainWindow()
{
    Field *field = static_cast<Field *>(_inter);
    buildGUI(field);
}

//-----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	const MainData MAIN_DATA = {
		"klickety",
		I18N_NOOP("Klickety"),
		I18N_NOOP("Klickety is an adaptation of the \"clickomania\" game"),
		"http://klickety.sourceforge.net/",
		I18N_NOOP("Removed blocks"),
		"1.0.3",
		"1.0.3 (5 August 2004)"
	};
	
	KlFactory factory(MAIN_DATA);
	factory.init(argc, argv);
	KHighscore::init(MAIN_DATA.appName);
	
	KLPieceInfo pieceInfo;
	KLHighscores highscores;
	
	if ( qApp->isSessionRestored() )
		RESTORE(KLMainWindow)
	else {
		KLMainWindow *mw = new KLMainWindow;
		mw->show();
	}
	return kapp->exec();
}

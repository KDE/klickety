/***************************************************************************
 *   Copyright (c) 1995, Eirik Eng                                         *
 *   hadacek@kde.org                                                       *
 *                                                                         *
 *   Copyright (c) 1996-2004, Nicolas Hadacek                              *
 *   hadacek@kde.org                                                       *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA               *
 ***************************************************************************/

#include "klfactory.h"



#include <kaboutdata.h>
#include <klocale.h>


#include "base/factory.h"
#include "klboard.h"
#include "field.h"



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



//
// Constructor / Destructor
//

KlFactory::KlFactory(const MainData &mainData) : BaseFactory(mainData, BASE_BOARD_INFO)
{
	_aboutData->addCredit("Dan Hill", I18N_NOOP("Icons"));
}



//
// Private
//
BaseBoard* KlFactory::createBoard(bool, QWidget *parent)
{
	return new KLBoard(parent);
}


BaseInterface* KlFactory::createInterface(QWidget *parent)
{
	return new Field(parent);
}

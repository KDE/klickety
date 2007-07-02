/***************************************************************************
 *   Copyright (c) 1995, Eirik Eng                                         *
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



//
// Constructor / Destructor
//

KlFactory::KlFactory(const MainData &mainData, const BaseBoardInfo info) : BaseFactory(mainData, info)
{
	_aboutData->addCredit(ki18n("Dan Hill"), ki18n("Icons"));
}



//
// Private
//
BaseBoard* KlFactory::createBoard(bool, QWidget *parent)
{
	return new KlBoard(parent);
}


BaseInterface* KlFactory::createInterface(QWidget *parent)
{
	return new Field(parent);
}

/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <QWidget>

#include "ui_gameconfig.h"

class GameConfig : public QWidget, private Ui::GameConfig
{
    public:
        explicit GameConfig( QWidget* parent = nullptr ) : QWidget(parent) {
            setupUi( this );
        }
};

#endif // GAMECONFIG_H

/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef CUSTOMGAMECONFIG_H
#define CUSTOMGAMECONFIG_H

#include <QWidget>

#include "ui_customgameconfig.h"

class CustomGameConfig : public QWidget, private Ui::CustomGameConfig
{
    public:
        explicit CustomGameConfig( QWidget* parent = nullptr ) : QWidget(parent) {
            setupUi( this );
        }
};

#endif // CUSTOMGAMECONFIG_H

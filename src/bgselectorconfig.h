/*
    SPDX-FileCopyrightText: 2010 Ni Hui <shuizhuyuanluo@126.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BGSELECTORCONFIG_H
#define BGSELECTORCONFIG_H

#include <QWidget>

#include "ui_bgselectorconfig.h"

class BgSelectorConfig : public QWidget, private Ui::BgSelectorConfig
{
    public:
        explicit BgSelectorConfig( QWidget* parent = nullptr ) : QWidget(parent) {
            setupUi( this );
        }
};

#endif // BGSELECTORCONFIG_H

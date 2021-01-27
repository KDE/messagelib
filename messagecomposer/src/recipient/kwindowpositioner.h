/*
    This file is part of KDE.

    SPDX-FileCopyrightText: 2005 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef KWINDOWPOSITIONER_H
#define KWINDOWPOSITIONER_H

#include <QWidget>

namespace MessageComposer
{
class KWindowPositioner : public QObject
{
    Q_OBJECT
public:
    enum Mode { Right, Bottom };

    KWindowPositioner(QWidget *master, QWidget *slave, Mode mode = Bottom);

    void reposition();

private:
    QWidget *const mMaster;
    QWidget *const mSlave;

    Mode mMode = Bottom;
};
}

#endif

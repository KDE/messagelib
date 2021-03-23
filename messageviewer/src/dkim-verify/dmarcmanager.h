/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QStringList>
namespace MessageViewer
{
class DMARCManager
{
public:
    DMARCManager();
    ~DMARCManager();

    static DMARCManager *self();

    void addNoDMarcServerAddress(const QString &address);
    Q_REQUIRED_RESULT bool isNoDMarcServerAddress(const QString &address) const;

private:
    void saveNoServerKeys();
    void loadNoServerKeys();
    QStringList mNoDMarcServer;
};
}


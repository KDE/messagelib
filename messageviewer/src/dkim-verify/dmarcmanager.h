/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/


#ifndef DMARCMANAGER_H
#define DMARCMANAGER_H

#include <QStringList>
namespace MessageViewer {

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

#endif // DMARCMANAGER_H

/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QStringList>
namespace MessageViewer
{
class DMARCManager
{
public:
    ~DMARCManager();

    static DMARCManager *self();

    void addNoDMarcServerAddress(const QString &address);
    [[nodiscard]] bool isNoDMarcServerAddress(const QString &address) const;

private:
    DMARCManager();
    void saveNoServerKeys();
    void loadNoServerKeys();
    QStringList mNoDMarcServer;
};
}

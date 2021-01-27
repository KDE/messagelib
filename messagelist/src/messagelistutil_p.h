/*
  This file is part of KMail, the KDE mail client.
  SPDX-FileCopyrightText: 2011-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGELISTUTIL_P_H
#define MESSAGELISTUTIL_P_H

#include <QObject>
#include <QString>

class QMenu;

namespace MessageList
{
namespace Util
{
Q_REQUIRED_RESULT QString messageSortingConfigName();
Q_REQUIRED_RESULT QString messageSortDirectionConfigName();
Q_REQUIRED_RESULT QString groupSortingConfigName();
Q_REQUIRED_RESULT QString groupSortDirectionConfigName();
Q_REQUIRED_RESULT QString messageUniqueIdConfigName();
Q_REQUIRED_RESULT QString storageModelSortOrderGroup();
Q_REQUIRED_RESULT QString storageModelThemesGroup();
Q_REQUIRED_RESULT QString storageModelAggregationsGroup();
Q_REQUIRED_RESULT QString setForStorageModelConfigName();
Q_REQUIRED_RESULT QString storageModelSelectedMessageGroup();
void fillViewMenu(QMenu *menu, QObject *receiver);
}
}

#endif /* MESSAGELISTUTIL_H */

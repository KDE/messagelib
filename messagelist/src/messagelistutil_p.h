/*
  This file is part of KMail, the KDE mail client.
  SPDX-FileCopyrightText: 2011-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QString>

class QMenu;

namespace MessageList
{
namespace Util
{
[[nodiscard]] QString messageSortingConfigName();
[[nodiscard]] QString messageSortDirectionConfigName();
[[nodiscard]] QString groupSortingConfigName();
[[nodiscard]] QString groupSortDirectionConfigName();
[[nodiscard]] QString messageUniqueIdConfigName();
[[nodiscard]] QString storageModelSortOrderGroup();
[[nodiscard]] QString storageModelThemesGroup();
[[nodiscard]] QString storageModelAggregationsGroup();
[[nodiscard]] QString setForStorageModelConfigName();
[[nodiscard]] QString storageModelSelectedMessageGroup();
void fillViewMenu(QMenu *menu, QObject *receiver);
}
}

/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filtersavedmanager.h"
#include "filter.h"
#include <KSharedConfig>
using namespace MessageList::Core;
FilterSavedManager::FilterSavedManager(QObject *parent)
    : QObject(parent)
{
}

FilterSavedManager::~FilterSavedManager()
{
}

void FilterSavedManager::saveFilter(MessageList::Core::Filter *filter, const QString &filtername)
{
    filter->save(KSharedConfig::openConfig(), filtername);
}

/*
  SPDX-FileCopyrightText: 2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "mdnwarningwidgetjob.h"

using namespace MessageViewer;
MDNWarningWidgetJob::MDNWarningWidgetJob(QObject *parent)
    : QObject{parent}
{
}

MDNWarningWidgetJob::~MDNWarningWidgetJob()
{
}

void MDNWarningWidgetJob::start()
{
    // TODO
}

const Akonadi::Item &MDNWarningWidgetJob::item() const
{
    return mItem;
}

void MDNWarningWidgetJob::setItem(const Akonadi::Item &newItem)
{
    mItem = newItem;
}

/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "remotecontentstatustypecombobox.h"
#include <KLocalizedString>
using namespace MessageViewer;
RemoteContentStatusTypeComboBox::RemoteContentStatusTypeComboBox(QWidget *parent)
    : QComboBox(parent)
{
    addItem(i18n("Authorized"), QVariant::fromValue(RemoteContentInfo::RemoteContentInfoStatus::Authorized));
    addItem(i18n("Blocked"), QVariant::fromValue(RemoteContentInfo::RemoteContentInfoStatus::Blocked));
}

RemoteContentStatusTypeComboBox::~RemoteContentStatusTypeComboBox() = default;

void RemoteContentStatusTypeComboBox::setStatus(MessageViewer::RemoteContentInfo::RemoteContentInfoStatus type)
{
    const int index = findData(QVariant::fromValue(type));
    if (index != -1) {
        setCurrentIndex(index);
    }
}

RemoteContentInfo::RemoteContentInfoStatus RemoteContentStatusTypeComboBox::status() const
{
    return currentData().value<RemoteContentInfo::RemoteContentInfoStatus>();
}

#include "moc_remotecontentstatustypecombobox.cpp"

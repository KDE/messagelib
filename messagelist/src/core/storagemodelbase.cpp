/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#include "core/storagemodelbase.h"
#include "messagelistsettings.h"
#include "messagelistutil_p.h"

using namespace MessageList::Core;

StorageModel::StorageModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

StorageModel::~StorageModel()
{
}

int StorageModel::initialUnreadRowCountGuess() const
{
    return rowCount(QModelIndex());
}

unsigned long StorageModel::preSelectedMessage() const
{
    const QString storageModelId = id();
    Q_ASSERT(!storageModelId.isEmpty());

    KConfigGroup conf(MessageListSettings::self()->config(), MessageList::Util::storageModelSelectedMessageGroup());

    // QVariant supports unsigned int OR unsigned long long int, NOT unsigned long int... doh...
    qulonglong defValue = 0;

    return conf.readEntry(MessageList::Util::messageUniqueIdConfigName().arg(storageModelId), defValue);
}

void StorageModel::savePreSelectedMessage(unsigned long uniqueIdOfMessage)
{
    const QString storageModelId = id();
    if (storageModelId.isEmpty()) {
        // This happens when deleting a collection, and this is called when switching away from it
        return;
    }

    KConfigGroup conf(MessageListSettings::self()->config(), MessageList::Util::storageModelSelectedMessageGroup());

    if (uniqueIdOfMessage) {
        // QVariant supports unsigned int OR unsigned long long int, NOT unsigned long int... doh...
        qulonglong val = uniqueIdOfMessage;

        conf.writeEntry(MessageList::Util::messageUniqueIdConfigName().arg(storageModelId), val);
    } else {
        conf.deleteEntry(MessageList::Util::messageUniqueIdConfigName().arg(storageModelId));
    }
}

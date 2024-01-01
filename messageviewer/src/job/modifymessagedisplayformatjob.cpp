/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "modifymessagedisplayformatjob.h"
#include "messageviewer_debug.h"

#include <Akonadi/ItemModifyJob>
#include <Akonadi/Session>

#include "viewer/messagedisplayformatattribute.h"

using namespace MessageViewer;
ModifyMessageDisplayFormatJob::ModifyMessageDisplayFormatJob(Akonadi::Session *session, QObject *parent)
    : QObject(parent)
    , mSession(session)
{
}

ModifyMessageDisplayFormatJob::~ModifyMessageDisplayFormatJob() = default;

void ModifyMessageDisplayFormatJob::setRemoteContent(bool remote)
{
    mRemoteContent = remote;
}

void ModifyMessageDisplayFormatJob::setMessageFormat(Viewer::DisplayFormatMessage format)
{
    mMessageFormat = format;
}

void ModifyMessageDisplayFormatJob::setResetFormat(bool resetFormat)
{
    mResetFormat = resetFormat;
}

void ModifyMessageDisplayFormatJob::start()
{
    if (mMessageItem.isValid()) {
        if (mResetFormat) {
            resetDisplayFormat();
        } else {
            modifyDisplayFormat();
        }
    } else {
        qCDebug(MESSAGEVIEWER_LOG) << " messageItem doesn't exist";
        deleteLater();
    }
}

void ModifyMessageDisplayFormatJob::setMessageItem(const Akonadi::Item &messageItem)
{
    mMessageItem = messageItem;
}

void ModifyMessageDisplayFormatJob::resetDisplayFormat()
{
    mMessageItem.removeAttribute<MessageViewer::MessageDisplayFormatAttribute>();
    auto modify = new Akonadi::ItemModifyJob(mMessageItem, mSession);
    modify->setIgnorePayload(true);
    modify->disableRevisionCheck();
    connect(modify, &KJob::result, this, &ModifyMessageDisplayFormatJob::slotModifyItemDone);
}

void ModifyMessageDisplayFormatJob::modifyDisplayFormat()
{
    auto attr = mMessageItem.attribute<MessageViewer::MessageDisplayFormatAttribute>(Akonadi::Item::AddIfMissing);
    attr->setRemoteContent(mRemoteContent);
    attr->setMessageFormat(mMessageFormat);
    auto modify = new Akonadi::ItemModifyJob(mMessageItem, mSession);
    modify->setIgnorePayload(true);
    modify->disableRevisionCheck();
    connect(modify, &KJob::result, this, &ModifyMessageDisplayFormatJob::slotModifyItemDone);
}

void ModifyMessageDisplayFormatJob::slotModifyItemDone(KJob *job)
{
    if (job && job->error()) {
        qCWarning(MESSAGEVIEWER_LOG) << " Error trying to change attribute:" << job->errorText();
    }
    deleteLater();
}

#include "moc_modifymessagedisplayformatjob.cpp"

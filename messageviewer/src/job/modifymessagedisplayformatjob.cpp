/*
   Copyright (C) 2014-2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "modifymessagedisplayformatjob.h"
#include "messageviewer_debug.h"

#include <AkonadiCore/ItemModifyJob>
#include <AkonadiCore/Session>

#include "viewer/messagedisplayformatattribute.h"

using namespace MessageViewer;
ModifyMessageDisplayFormatJob::ModifyMessageDisplayFormatJob(Akonadi::Session *session, QObject *parent)
    : QObject(parent)
    , mSession(session)
{
}

ModifyMessageDisplayFormatJob::~ModifyMessageDisplayFormatJob()
{
}

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
    Akonadi::ItemModifyJob *modify = new Akonadi::ItemModifyJob(mMessageItem, mSession);
    modify->setIgnorePayload(true);
    modify->disableRevisionCheck();
    connect(modify, &KJob::result, this, &ModifyMessageDisplayFormatJob::slotModifyItemDone);
}

void ModifyMessageDisplayFormatJob::modifyDisplayFormat()
{
    MessageViewer::MessageDisplayFormatAttribute *attr
        = mMessageItem.attribute<MessageViewer::MessageDisplayFormatAttribute>(
              Akonadi::Item::AddIfMissing);
    attr->setRemoteContent(mRemoteContent);
    attr->setMessageFormat(mMessageFormat);
    Akonadi::ItemModifyJob *modify = new Akonadi::ItemModifyJob(mMessageItem, mSession);
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

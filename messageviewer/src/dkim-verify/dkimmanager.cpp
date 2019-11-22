/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

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

#include "dkimmanager.h"
#include "messageviewer_dkimcheckerdebug.h"
#include "dkimresultattribute.h"
#include "dkimcheckfulljob.h"
#include "settings/messageviewersettings.h"
#include <AkonadiCore/AttributeFactory>
using namespace MessageViewer;
DKIMManager::DKIMManager(QObject *parent)
    : QObject(parent)
{
    Akonadi::AttributeFactory::registerAttribute<MessageViewer::DKIMResultAttribute>();
}

DKIMManager::~DKIMManager()
{
}

DKIMManager *DKIMManager::self()
{
    static DKIMManager s_self;
    return &s_self;
}

void DKIMManager::clearInfoWidget()
{
    Q_EMIT clearInfo();
}

DKIMCheckPolicy DKIMManager::policy() const
{
    return mCheckPolicy;
}

void DKIMManager::setPolicy(const DKIMCheckPolicy &policy)
{
    mCheckPolicy = policy;
}

void DKIMManager::checkDKim(const Akonadi::Item &item)
{
    if (mCheckPolicy.saveDkimResult()) {
        if (item.hasAttribute<MessageViewer::DKIMResultAttribute>()) {
            const MessageViewer::DKIMResultAttribute *const attr
                = item.attribute<MessageViewer::DKIMResultAttribute>();
            if (attr) {
                DKIMCheckSignatureJob::CheckSignatureResult checkResult;
                checkResult.error = static_cast<DKIMCheckSignatureJob::DKIMError>(attr->error());
                checkResult.warning = static_cast<DKIMCheckSignatureJob::DKIMWarning>(attr->warning());
                checkResult.status = static_cast<DKIMCheckSignatureJob::DKIMStatus>(attr->status());
                qCDebug(MESSAGEVIEWER_DKIMCHECKER_LOG) << "result : status " << checkResult.status << " error : " << checkResult.error << " warning " << checkResult.warning;
                Q_EMIT result(checkResult, item.id());
                return;
            }
        }
    }
    checkFullInfo(item);
}

void DKIMManager::checkFullInfo(const Akonadi::Item &item)
{
    DKIMCheckFullJob *job = new DKIMCheckFullJob(this);
    job->setPolicy(mCheckPolicy);
    connect(job, &DKIMCheckFullJob::result, this, &DKIMManager::result);
    job->startCheckFullInfo(item);
}

void DKIMManager::checkDKim(const KMime::Message::Ptr &message)
{
    DKIMCheckFullJob *job = new DKIMCheckFullJob(this);
    job->setPolicy(mCheckPolicy);
    connect(job, &DKIMCheckFullJob::result, this, &DKIMManager::result);
    job->startCheckFullInfo(message);
}

/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "mailcheckphishingurljob.h"

using namespace MessageViewer;

MailCheckPhishingUrlJob::MailCheckPhishingUrlJob(QObject *parent)
    : QObject(parent)
{
    mCheckPhishingJob = new WebEngineViewer::CheckPhishingUrlJob;
    connect(mCheckPhishingJob.data(), &WebEngineViewer::CheckPhishingUrlJob::result, this, &MailCheckPhishingUrlJob::slotCheckPhishingUrlDone);
}

MailCheckPhishingUrlJob::~MailCheckPhishingUrlJob()
{
    if (mCheckPhishingJob) {
        disconnect(mCheckPhishingJob.data(), &WebEngineViewer::CheckPhishingUrlJob::result, this, &MailCheckPhishingUrlJob::slotCheckPhishingUrlDone);
        mCheckPhishingJob = Q_NULLPTR;
    }
}

void MailCheckPhishingUrlJob::start()
{
    mCheckPhishingJob->start();
}

void MailCheckPhishingUrlJob::setItem(const Akonadi::Item &item)
{
    mItem = item;
}

void MailCheckPhishingUrlJob::setUrl(const QUrl &url)
{
    mCheckPhishingJob->setUrl(url);
}

void MailCheckPhishingUrlJob::slotCheckPhishingUrlDone(WebEngineViewer::CheckPhishingUrlUtil::UrlStatus status, const QUrl &url, uint verifyCacheAfterThisTime)
{
    Q_EMIT result(status, url, mItem, verifyCacheAfterThisTime);
    deleteLater();
}

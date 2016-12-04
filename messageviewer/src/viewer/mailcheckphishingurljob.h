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

#ifndef MAILCHECKPHISHINGURLJOB_H
#define MAILCHECKPHISHINGURLJOB_H

#include <QObject>
#include <AkonadiCore/Item>
#include <WebEngineViewer/CheckPhishingUrlJob>
#include <QPointer>

namespace MessageViewer
{
class MailCheckPhishingUrlJob : public QObject
{
    Q_OBJECT
public:
    explicit MailCheckPhishingUrlJob(QObject *parent);
    ~MailCheckPhishingUrlJob();

    void start();

    void setItem(const Akonadi::Item &item);
    void setUrl(const QUrl &url);

Q_SIGNALS:
    void result(WebEngineViewer::CheckPhishingUrlJob::UrlStatus status, const QUrl &url, const Akonadi::Item &item, uint verifyCacheAfterThisTime);

private Q_SLOTS:
    void slotCheckPhishingUrlDone(WebEngineViewer::CheckPhishingUrlJob::UrlStatus status, const QUrl &url, uint verifyCacheAfterThisTime);

private:
    Akonadi::Item mItem;
    QPointer<WebEngineViewer::CheckPhishingUrlJob> mCheckPhishingJob;
};
}
#endif // MAILCHECKPHISHINGURLJOB_H

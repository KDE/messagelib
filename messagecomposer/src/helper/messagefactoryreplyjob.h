/*
   Copyright (C) 2017-2019 Laurent Montel <montel@kde.org>

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

#ifndef MESSAGEFACTORYREPLYJOB_H
#define MESSAGEFACTORYREPLYJOB_H

#include <QObject>
#include <KMime/Message>
#include <KIdentityManagement/IdentityManager>
#include <AkonadiCore/Collection>

namespace MessageComposer {
class MessageFactoryReplyJob : public QObject
{
    Q_OBJECT
public:
    explicit MessageFactoryReplyJob(QObject *parent = nullptr);
    ~MessageFactoryReplyJob();

    void start();

    void setMsg(const KMime::Message::Ptr &msg);

    void setTemplate(const QString &tmpl);

    void setSelection(const QString &selection);

    void setOrigMsg(const KMime::Message::Ptr &origMsg);

    void setIdentityManager(KIdentityManagement::IdentityManager *identityManager);

    void setReplyAll(bool replyAll);

    void setCollection(const Akonadi::Collection &collection);

Q_SIGNALS:
    void replyDone(const KMime::Message::Ptr &msg, bool replyAll);

private:
    void slotReplyDone();
    QString mSelection;
    QString mTemplate;
    KMime::Message::Ptr mMsg = nullptr;
    KMime::Message::Ptr mOrigMsg = nullptr;
    Akonadi::Collection mCollection;
    bool mReplyAll = false;
    KIdentityManagement::IdentityManager *mIdentityManager = nullptr;
};
}
#endif // MESSAGEFACTORYREPLYJOB_H

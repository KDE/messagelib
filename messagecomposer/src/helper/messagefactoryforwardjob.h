/*
   Copyright (c) 2017 Laurent Montel <montel@kde.org>

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

#ifndef MESSAGEFACTORYFORWARDJOB_H
#define MESSAGEFACTORYFORWARDJOB_H

#include <QObject>
#include <KMime/Message>
#include <AkonadiCore/Collection>
namespace KIdentityManagement
{
class IdentityManager;
}

namespace MessageComposer
{
class MessageFactoryForwardJob : public QObject
{
    Q_OBJECT
public:
    explicit MessageFactoryForwardJob(QObject *parent = nullptr);
    ~MessageFactoryForwardJob();
    void start();
    void setMsg(const KMime::Message::Ptr &msg);

    void setTemplate(const QString &tmpl);

    void setSelection(const QString &selection);

    void setOrigMsg(const KMime::Message::Ptr &origMsg);

    void setIdentityManager(KIdentityManagement::IdentityManager *identityManager);

Q_SIGNALS:
    void forwardDone(const KMime::Message::Ptr &msg);

private:
    void slotParsingDone();

    QString mSelection;
    QString mTemplate;
    KMime::Message::Ptr mMsg;
    KMime::Message::Ptr mOrigMsg;
    Akonadi::Collection mCollection;
    KIdentityManagement::IdentityManager *mIdentityManager;
};
}

#endif // MESSAGEFACTORYFORWARDJOB_H

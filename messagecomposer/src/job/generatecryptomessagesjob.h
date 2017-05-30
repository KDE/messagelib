/*
  Copyright (c) 2017 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef GENERATECRYPTOMESSAGESJOB_H
#define GENERATECRYPTOMESSAGESJOB_H

#include <QObject>
#include <KIdentityManagement/Identity>
#include <MessageCore/AttachmentPart>
#include <MessageComposer/Composer>

namespace MessageComposer
{
class GenerateCryptoMessagesJob : public QObject
{
    Q_OBJECT
public:
    explicit GenerateCryptoMessagesJob(QObject *parent = nullptr);
    ~GenerateCryptoMessagesJob();

    QList<MessageComposer::Composer *> generateCryptoMessages(bool &wasCanceled);

    void setIdentity(const KIdentityManagement::Identity &id);
    void setAttachmentList(const MessageCore::AttachmentPart::List &attachmentList);

    void setSign(bool sign);

    void setEncrypt(bool encrypt);

Q_SIGNALS:
    void failed(const QString &msg);

private:
    KIdentityManagement::Identity mId;
    MessageCore::AttachmentPart::List mAttachmentList;
    bool mSign;
    bool mEncrypt;
};
}

#endif // GENERATECRYPTOMESSAGESJOB_H

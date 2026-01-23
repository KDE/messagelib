/*
  SPDX-FileCopyrightText: 2015-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "MessageCore/AttachmentLoadJob"
#include "messagecomposer_export.h"
#include <Akonadi/Item>
namespace MessageComposer
{
class AttachmentVcardFromAddressBookJobPrivate;
/*!
 \class MessageComposer::AttachmentVcardFromAddressBookJob
 \inmodule MessageComposer
 \inheaderfile MessageComposer/AttachmentVcardFromAddressBookJob
 \author Laurent Montel <montel@kde.org>

 The AttachmentVcardFromAddressBookJob class.
 */
class MESSAGECOMPOSER_EXPORT AttachmentVcardFromAddressBookJob : public MessageCore::AttachmentLoadJob
{
    Q_OBJECT
public:
    /*!
     */
    explicit AttachmentVcardFromAddressBookJob(const Akonadi::Item &item, QObject *parent = nullptr);
    /*!
     */
    ~AttachmentVcardFromAddressBookJob() override;

protected Q_SLOTS:
    /*!
     */
    void doStart() override;

private:
    MESSAGECOMPOSER_NO_EXPORT void slotExpandGroupResult(KJob *job);
    MESSAGECOMPOSER_NO_EXPORT void invalidContact();
    MESSAGECOMPOSER_NO_EXPORT void addAttachment(const QByteArray &data, const QString &attachmentName);
    std::unique_ptr<AttachmentVcardFromAddressBookJobPrivate> const d;
};
}

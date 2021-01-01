/*
  SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ATTACHMENTVCARDFROMADDRESSBOOKJOB_H
#define ATTACHMENTVCARDFROMADDRESSBOOKJOB_H

#include "MessageCore/AttachmentLoadJob"
#include <AkonadiCore/Item>
#include "messagecomposer_export.h"
namespace MessageComposer {
class AttachmentVcardFromAddressBookJobPrivate;
/**
 * @brief The AttachmentVcardFromAddressBookJob class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT AttachmentVcardFromAddressBookJob : public MessageCore::AttachmentLoadJob
{
    Q_OBJECT
public:
    explicit AttachmentVcardFromAddressBookJob(const Akonadi::Item &item, QObject *parent = nullptr);
    ~AttachmentVcardFromAddressBookJob() override;

protected Q_SLOTS:
    void doStart() override;

private Q_SLOTS:
    void slotExpandGroupResult(KJob *job);

private:
    void invalidContact();
    void addAttachment(const QByteArray &data, const QString &attachmentName);
    AttachmentVcardFromAddressBookJobPrivate *const d;
};
}
#endif // ATTACHMENTVCARDFROMADDRESSBOOKJOB_H

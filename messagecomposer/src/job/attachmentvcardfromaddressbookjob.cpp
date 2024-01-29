/*
  SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "attachmentvcardfromaddressbookjob.h"
#include <Akonadi/ContactGroupExpandJob>
#include <KContacts/Addressee>
#include <KContacts/ContactGroup>
#include <KContacts/VCardConverter>
#include <KLocalizedString>

using namespace MessageComposer;

class MessageComposer::AttachmentVcardFromAddressBookJobPrivate
{
public:
    explicit AttachmentVcardFromAddressBookJobPrivate(const Akonadi::Item &item)
        : mItem(item)
    {
    }

    Akonadi::Item mItem;
};

AttachmentVcardFromAddressBookJob::AttachmentVcardFromAddressBookJob(const Akonadi::Item &item, QObject *parent)
    : MessageCore::AttachmentLoadJob(parent)
    , d(new MessageComposer::AttachmentVcardFromAddressBookJobPrivate(item))
{
}

AttachmentVcardFromAddressBookJob::~AttachmentVcardFromAddressBookJob() = default;

void AttachmentVcardFromAddressBookJob::addAttachment(const QByteArray &data, const QString &attachmentName)
{
    MessageCore::AttachmentPart::Ptr attachment = MessageCore::AttachmentPart::Ptr(new MessageCore::AttachmentPart());
    if (!data.isEmpty()) {
        attachment->setName(attachmentName);
        attachment->setFileName(attachmentName);
        attachment->setData(data);
        attachment->setMimeType("text/x-vcard");
        // TODO what about the other fields?
    }
    setAttachmentPart(attachment);
    emitResult(); // Success.
}

void AttachmentVcardFromAddressBookJob::doStart()
{
    if (d->mItem.isValid()) {
        if (d->mItem.hasPayload<KContacts::Addressee>()) {
            const auto contact = d->mItem.payload<KContacts::Addressee>();
            if (contact.isEmpty()) {
                invalidContact();
            } else {
                const QString contactRealName(contact.realName());
                const QString attachmentName = (contactRealName.isEmpty() ? QStringLiteral("vcard") : contactRealName) + QLatin1StringView(".vcf");

                QByteArray data = d->mItem.payloadData();
                // Workaround about broken kaddressbook fields.
                KContacts::adaptIMAttributes(data);
                addAttachment(data, attachmentName);
            }
        } else if (d->mItem.hasPayload<KContacts::ContactGroup>()) {
            const auto group = d->mItem.payload<KContacts::ContactGroup>();
            const QString groupName(group.name());
            const QString attachmentName = (groupName.isEmpty() ? QStringLiteral("vcard") : groupName) + QLatin1StringView(".vcf");
            auto expandJob = new Akonadi::ContactGroupExpandJob(group, this);
            expandJob->setProperty("groupName", attachmentName);
            connect(expandJob, &KJob::result, this, &AttachmentVcardFromAddressBookJob::slotExpandGroupResult);
            expandJob->start();
        } else {
            setError(KJob::UserDefinedError);
            setErrorText(i18n("Unknown Contact Type"));
            emitResult();
        }
    } else {
        invalidContact();
    }
}

void AttachmentVcardFromAddressBookJob::invalidContact()
{
    setError(KJob::UserDefinedError);
    setErrorText(i18n("Invalid Contact"));
    emitResult();
}

void AttachmentVcardFromAddressBookJob::slotExpandGroupResult(KJob *job)
{
    auto expandJob = qobject_cast<Akonadi::ContactGroupExpandJob *>(job);
    Q_ASSERT(expandJob);

    const QString attachmentName = expandJob->property("groupName").toString();
    KContacts::VCardConverter converter;
    const QByteArray groupData = converter.exportVCards(expandJob->contacts(), KContacts::VCardConverter::v3_0);
    if (!groupData.isEmpty()) {
        addAttachment(groupData, attachmentName);
    } else {
        setError(KJob::UserDefinedError);
        setErrorText(i18n("Impossible to generate vCard."));
        emitResult();
    }
}

#include "moc_attachmentvcardfromaddressbookjob.cpp"

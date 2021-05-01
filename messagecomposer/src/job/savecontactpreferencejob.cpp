/*
   SPDX-FileCopyrightText: 2014-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "savecontactpreferencejob.h"

#include <KJob>
#include <KLocalizedString>

#include <QInputDialog>

#include <Akonadi/Contact/ContactSearchJob>
#include <AkonadiCore/ItemCreateJob>
#include <AkonadiCore/ItemModifyJob>
#include <AkonadiWidgets/CollectionDialog>

#include "messagecomposer_debug.h"
#include <QPointer>

using namespace MessageComposer;

SaveContactPreferenceJob::SaveContactPreferenceJob(const QString &email, const Kleo::KeyResolver::ContactPreferences &pref, QObject *parent)
    : QObject(parent)
    , mEmail(email)
    , mPref(pref)
{
}

SaveContactPreferenceJob::~SaveContactPreferenceJob()
{
}

void SaveContactPreferenceJob::start()
{
    auto job = new Akonadi::ContactSearchJob(this);
    connect(job, &Akonadi::ContactSearchJob::result, this, &SaveContactPreferenceJob::slotSearchContact);
    job->setLimit(1);
    job->setQuery(Akonadi::ContactSearchJob::Email, mEmail);
    job->start();
}

void SaveContactPreferenceJob::slotSearchContact(KJob *job)
{
    auto contactSearchJob = qobject_cast<Akonadi::ContactSearchJob *>(job);

    const Akonadi::Item::List items = contactSearchJob->items();

    if (items.isEmpty()) {
        bool ok = true;
        const QString fullName = QInputDialog::getText(nullptr,
                                                       i18n("Name Selection"),
                                                       i18n("Which name shall the contact '%1' have in your address book?", mEmail),
                                                       QLineEdit::Normal,
                                                       QString(),
                                                       &ok);
        if (!ok) {
            deleteLater();
            return;
        }

        QPointer<Akonadi::CollectionDialog> dlg = new Akonadi::CollectionDialog(Akonadi::CollectionDialog::KeepTreeExpanded);
        dlg->setMimeTypeFilter(QStringList() << KContacts::Addressee::mimeType());
        dlg->setAccessRightsFilter(Akonadi::Collection::CanCreateItem);
        dlg->setDescription(i18n("Select the address book folder to store the new contact in:"));
        if (!dlg->exec()) {
            delete dlg;
            deleteLater();
            return;
        }

        const Akonadi::Collection targetCollection = dlg->selectedCollection();
        delete dlg;

        KContacts::Addressee contact;
        contact.setNameFromString(fullName);
        contact.insertEmail(mEmail, true);
        writeCustomContactProperties(contact, mPref);

        Akonadi::Item item(KContacts::Addressee::mimeType());
        item.setPayload<KContacts::Addressee>(contact);

        auto job = new Akonadi::ItemCreateJob(item, targetCollection);
        connect(job, &Akonadi::ContactSearchJob::result, this, &SaveContactPreferenceJob::slotModifyCreateItem);
    } else {
        Akonadi::Item item = items.first();

        auto contact = item.payload<KContacts::Addressee>();
        writeCustomContactProperties(contact, mPref);

        item.setPayload<KContacts::Addressee>(contact);

        auto job = new Akonadi::ItemModifyJob(item);
        connect(job, &Akonadi::ContactSearchJob::result, this, &SaveContactPreferenceJob::slotModifyCreateItem);
    }
}

void SaveContactPreferenceJob::writeCustomContactProperties(KContacts::Addressee &contact, const Kleo::KeyResolver::ContactPreferences &pref) const
{
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"),
                         QStringLiteral("CRYPTOENCRYPTPREF"),
                         QLatin1String(Kleo::encryptionPreferenceToString(pref.encryptionPreference)));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"),
                         QStringLiteral("CRYPTOSIGNPREF"),
                         QLatin1String(Kleo::signingPreferenceToString(pref.signingPreference)));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"),
                         QStringLiteral("CRYPTOPROTOPREF"),
                         QLatin1String(cryptoMessageFormatToString(pref.cryptoMessageFormat)));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("OPENPGPFP"), pref.pgpKeyFingerprints.join(QLatin1Char(',')));
    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("SMIMEFP"), pref.smimeCertFingerprints.join(QLatin1Char(',')));
}

void SaveContactPreferenceJob::slotModifyCreateItem(KJob *job)
{
    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "modify item failed " << job->errorString();
    }
    deleteLater();
}

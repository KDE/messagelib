/*
   SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "savecontactpreferencejob.h"

#include <KJob>
#include <KLocalizedString>

#include <QInputDialog>

#include <Akonadi/CollectionDialog>
#include <Akonadi/ContactSearchJob>
#include <Akonadi/ItemCreateJob>
#include <Akonadi/ItemModifyJob>

#include "messagecomposer_debug.h"
#include <QPointer>

using namespace MessageComposer;

class MessageComposer::SaveContactPreferenceJobPrivate
{
public:
    explicit SaveContactPreferenceJobPrivate(SaveContactPreferenceJob *qq, const QString &email, const ContactPreference &pref)
        : q_ptr(qq)
        , mEmail(email)
        , mPref(pref)
    {
    }

    SaveContactPreferenceJob *const q_ptr;
    Q_DECLARE_PUBLIC(SaveContactPreferenceJob)

    const QString mEmail;
    const ContactPreference mPref;
};

SaveContactPreferenceJob::SaveContactPreferenceJob(const QString &email, const ContactPreference &pref, QObject *parent)
    : QObject(parent)
    , d_ptr(std::unique_ptr<SaveContactPreferenceJobPrivate>(new SaveContactPreferenceJobPrivate(this, email, pref)))
{
}

SaveContactPreferenceJob::~SaveContactPreferenceJob() = default;

void SaveContactPreferenceJob::start()
{
    Q_D(SaveContactPreferenceJob);

    auto job = new Akonadi::ContactSearchJob(this);
    connect(job, &Akonadi::ContactSearchJob::result, this, &SaveContactPreferenceJob::slotSearchContact);
    job->setLimit(1);
    job->setQuery(Akonadi::ContactSearchJob::Email, d->mEmail);
    job->start();
}

void SaveContactPreferenceJob::slotSearchContact(KJob *job)
{
    Q_D(SaveContactPreferenceJob);
    auto contactSearchJob = qobject_cast<Akonadi::ContactSearchJob *>(job);

    const Akonadi::Item::List items = contactSearchJob->items();

    if (items.isEmpty()) {
        bool ok = true;
        const QString fullName = QInputDialog::getText(nullptr,
                                                       i18nc("@title:window", "Name Selection"),
                                                       i18n("Which name shall the contact '%1' have in your address book?", d->mEmail),
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
        KContacts::Email email(d->mEmail);
        email.setPreferred(true);
        contact.addEmail(email);
        d->mPref.fillAddressee(contact);

        Akonadi::Item item(KContacts::Addressee::mimeType());
        item.setPayload<KContacts::Addressee>(contact);

        auto createJob = new Akonadi::ItemCreateJob(item, targetCollection);
        connect(createJob, &Akonadi::ContactSearchJob::result, this, &SaveContactPreferenceJob::slotModifyCreateItem);
    } else {
        Akonadi::Item item = items.first();

        auto contact = item.payload<KContacts::Addressee>();
        d->mPref.fillAddressee(contact);

        item.setPayload<KContacts::Addressee>(contact);

        auto itemModifyJob = new Akonadi::ItemModifyJob(item);
        connect(itemModifyJob, &Akonadi::ContactSearchJob::result, this, &SaveContactPreferenceJob::slotModifyCreateItem);
    }
}

void SaveContactPreferenceJob::slotModifyCreateItem(KJob *job)
{
    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "modify item failed " << job->errorString();
    }
    deleteLater();
}

#include "moc_savecontactpreferencejob.cpp"

/*
 * SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "contactdisplaymessagememento.h"
#include "messageviewer_debug.h"
#include <Akonadi/ContactSearchJob>
#include <Gravatar/GravatarResolvUrlJob>
#include <KIO/TransferJob>
#include <gravatar/gravatarsettings.h>

using namespace MessageViewer;

ContactDisplayMessageMemento::ContactDisplayMessageMemento(const QString &emailAddress)
    : QObject(nullptr)
    , mEmailAddress(emailAddress)
{
    if (!emailAddress.isEmpty()) {
        mSearchJob = new Akonadi::ContactSearchJob();
        mSearchJob->setQuery(Akonadi::ContactSearchJob::Email, emailAddress.toLower(), Akonadi::ContactSearchJob::ExactMatch);
        connect(mSearchJob.data(), &Akonadi::ContactSearchJob::result, this, &ContactDisplayMessageMemento::slotSearchJobFinished);
    } else {
        mFinished = true;
    }
}

ContactDisplayMessageMemento::~ContactDisplayMessageMemento()
{
    if (mSearchJob) {
        disconnect(mSearchJob.data(), &Akonadi::ContactSearchJob::result, this, &ContactDisplayMessageMemento::slotSearchJobFinished);
        mSearchJob->kill();
    }
}

void ContactDisplayMessageMemento::slotSearchJobFinished(KJob *job)
{
    mFinished = true;
    auto searchJob = static_cast<Akonadi::ContactSearchJob *>(job);
    if (searchJob->error()) {
        qCWarning(MESSAGEVIEWER_LOG) << "Unable to fetch contact:" << searchJob->errorText();
        Q_EMIT update(MimeTreeParser::Delayed);
        return;
    }

    const int contactSize(searchJob->contacts().size());
    if (contactSize >= 1) {
        if (contactSize > 1) {
            qCWarning(MESSAGEVIEWER_LOG) << " more than 1 contact was found we return first contact";
        }

        const KContacts::Addressee addressee = searchJob->contacts().at(0);
        processAddress(addressee);
        if (!searchPhoto(searchJob->contacts())) {
            // We have a data raw => we can update message
            if (mPhoto.isIntern()) {
                Q_EMIT update(MimeTreeParser::Delayed);
            } else {
                const QUrl url = QUrl::fromUserInput(mPhoto.url(), QString(), QUrl::AssumeLocalFile);
                if (!url.isEmpty()) {
                    bool ok = false;
                    QImage image;

                    if (url.isLocalFile()) {
                        if (image.load(url.toLocalFile())) {
                            ok = true;
                        }
                    } else {
                        QByteArray imageData;
                        KIO::TransferJob *jobTransfert = KIO::get(url, KIO::NoReload);
                        QObject::connect(jobTransfert, &KIO::TransferJob::data, this, [&imageData](KIO::Job *, const QByteArray &data) {
                            imageData.append(data);
                        });
                        if (jobTransfert->exec()) {
                            if (image.loadFromData(imageData)) {
                                ok = true;
                            }
                        }
                    }
                    if (ok) {
                        mImageFromUrl = image;
                        Q_EMIT update(MimeTreeParser::Delayed);
                    }
                }
            }
        }
    }
    if (mPhoto.isEmpty() && mPhoto.url().isEmpty()) {
        // No url, no photo => search gravatar
        if (Gravatar::GravatarSettings::self()->gravatarSupportEnabled()) {
            auto resolvUrljob = new Gravatar::GravatarResolvUrlJob(this);
            resolvUrljob->setEmail(mEmailAddress);
            resolvUrljob->setUseDefaultPixmap(Gravatar::GravatarSettings::self()->gravatarUseDefaultImage());
            resolvUrljob->setFallbackGravatar(Gravatar::GravatarSettings::self()->fallbackToGravatar());
            resolvUrljob->setUseLibravatar(Gravatar::GravatarSettings::self()->libravatarSupportEnabled());
            if (resolvUrljob->canStart()) {
                connect(resolvUrljob, &Gravatar::GravatarResolvUrlJob::finished, this, &ContactDisplayMessageMemento::slotGravatarResolvUrlFinished);
                resolvUrljob->start();
            } else {
                resolvUrljob->deleteLater();
            }
        }
    }
}

bool ContactDisplayMessageMemento::finished() const
{
    return mFinished;
}

void ContactDisplayMessageMemento::detach()
{
    disconnect(this, SIGNAL(update(MimeTreeParser::UpdateMode)), nullptr, nullptr);
    disconnect(this, SIGNAL(changeDisplayMail(Viewer::DisplayFormatMessage, bool)), nullptr, nullptr);
}

bool ContactDisplayMessageMemento::searchPhoto(const KContacts::AddresseeList &list)
{
    bool foundPhoto = false;
    for (const KContacts::Addressee &addressee : list) {
        const KContacts::Picture photo = addressee.photo();
        if (!photo.isEmpty()) {
            mPhoto = photo;
            foundPhoto = true;
            break;
        }
    }
    return foundPhoto;
}

QImage ContactDisplayMessageMemento::imageFromUrl() const
{
    return mImageFromUrl;
}

QPixmap ContactDisplayMessageMemento::gravatarPixmap() const
{
    return mGravatarPixmap;
}

void ContactDisplayMessageMemento::processAddress(const KContacts::Addressee &addressee)
{
    Viewer::DisplayFormatMessage forceDisplayTo = Viewer::UseGlobalSetting;
    bool mailAllowToRemoteContent = false;
    const QStringList customs = addressee.customs();
    for (const QString &custom : customs) {
        if (custom.contains(QLatin1StringView("MailPreferedFormatting"))) {
            const QString value = addressee.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("MailPreferedFormatting"));
            if (value == QLatin1StringView("TEXT")) {
                forceDisplayTo = Viewer::Text;
            } else if (value == QLatin1StringView("HTML")) {
                forceDisplayTo = Viewer::Html;
            } else {
                forceDisplayTo = Viewer::UseGlobalSetting;
            }
        } else if (custom.contains(QLatin1StringView("MailAllowToRemoteContent"))) {
            const QString value = addressee.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("MailAllowToRemoteContent"));
            mailAllowToRemoteContent = (value == QLatin1StringView("TRUE"));
        }
    }
    Q_EMIT changeDisplayMail(forceDisplayTo, mailAllowToRemoteContent);
}

KContacts::Picture ContactDisplayMessageMemento::photo() const
{
    return mPhoto;
}

void ContactDisplayMessageMemento::slotGravatarResolvUrlFinished(Gravatar::GravatarResolvUrlJob *job)
{
    if (job && job->hasGravatar()) {
        mGravatarPixmap = job->pixmap();
        Q_EMIT update(MimeTreeParser::Delayed);
    }
}

#include "moc_contactdisplaymessagememento.cpp"

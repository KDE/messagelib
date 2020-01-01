/*
 * Copyright (C) 2012-2020 Laurent Montel <montel@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "contactdisplaymessagememento.h"
#include "messageviewer_debug.h"
#include "Gravatar/GravatarResolvUrlJob"
#include "gravatarsettings.h"
#include "settings/messageviewersettings.h"
#include <Akonadi/Contact/ContactSearchJob>
#include <kio/transferjob.h>

using namespace MessageViewer;

ContactDisplayMessageMemento::ContactDisplayMessageMemento(const QString &emailAddress)
    : QObject(nullptr)
    , mEmailAddress(emailAddress)
{
    if (!emailAddress.isEmpty()) {
        mSearchJob = new Akonadi::ContactSearchJob();
        mSearchJob->setQuery(Akonadi::ContactSearchJob::Email,
                             emailAddress.toLower(), Akonadi::ContactSearchJob::ExactMatch);
        connect(
            mSearchJob.data(), &Akonadi::ContactSearchJob::result, this,
            &ContactDisplayMessageMemento::slotSearchJobFinished);
    } else {
        mFinished = true;
    }
}

ContactDisplayMessageMemento::~ContactDisplayMessageMemento()
{
    if (mSearchJob) {
        disconnect(
            mSearchJob.data(), &Akonadi::ContactSearchJob::result, this,
            &ContactDisplayMessageMemento::slotSearchJobFinished);
        mSearchJob->kill();
    }
}

void ContactDisplayMessageMemento::slotSearchJobFinished(KJob *job)
{
    mFinished = true;
    Akonadi::ContactSearchJob *searchJob = static_cast<Akonadi::ContactSearchJob *>(job);
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
        searchPhoto(searchJob->contacts());
        if (!mPhoto.isEmpty()) {
            //We have a data raw => we can update message
            if (mPhoto.isIntern()) {
                Q_EMIT update(MimeTreeParser::Delayed);
            } else {
                QUrl url = QUrl::fromUserInput(mPhoto.url(), QString(), QUrl::AssumeLocalFile);
                QImage image;
                bool ok = false;

                if (url.isLocalFile()) {
                    if (image.load(url.toLocalFile())) {
                        ok = true;
                    }
                } else {
                    QByteArray imageData;
                    KIO::TransferJob *job = KIO::get(url, KIO::NoReload);
                    QObject::connect(job, &KIO::TransferJob::data,
                                     [&imageData](KIO::Job *, const QByteArray &data) {
                        imageData.append(data);
                    });
                    if (job->exec()) {
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
    if (mPhoto.isEmpty() && mPhoto.url().isEmpty()) {
        // No url, no photo => search gravatar
        if (Gravatar::GravatarSettings::self()->gravatarSupportEnabled()) {
            Gravatar::GravatarResolvUrlJob *job = new Gravatar::GravatarResolvUrlJob(this);
            job->setEmail(mEmailAddress);
            job->setUseDefaultPixmap(
                Gravatar::GravatarSettings::self()->gravatarUseDefaultImage());
            if (job->canStart()) {
                connect(job, &Gravatar::GravatarResolvUrlJob::finished, this,
                        &ContactDisplayMessageMemento::slotGravatarResolvUrlFinished);
                job->start();
            } else {
                job->deleteLater();
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
    disconnect(this, SIGNAL(changeDisplayMail(Viewer::DisplayFormatMessage,bool)), nullptr,
               nullptr);
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
        if (custom.contains(QLatin1String("MailPreferedFormatting"))) {
            const QString value
                = addressee.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral(
                                       "MailPreferedFormatting"));
            if (value == QLatin1String("TEXT")) {
                forceDisplayTo = Viewer::Text;
            } else if (value == QLatin1String("HTML")) {
                forceDisplayTo = Viewer::Html;
            } else {
                forceDisplayTo = Viewer::UseGlobalSetting;
            }
        } else if (custom.contains(QLatin1String("MailAllowToRemoteContent"))) {
            const QString value
                = addressee.custom(QStringLiteral("KADDRESSBOOK"), QStringLiteral(
                                       "MailAllowToRemoteContent"));
            mailAllowToRemoteContent = (value == QLatin1String("TRUE"));
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

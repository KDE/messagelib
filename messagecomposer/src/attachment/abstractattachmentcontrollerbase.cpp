/*
 * This file is part of KMail.
 * SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
 *
 * Parts based on KMail code by:
 * Various authors.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "abstractattachmentcontrollerbase.h"
#include "abstractattachmentcontrollerbase_p.h"

void AbstractAttachmentControllerBase::setSelectedParts(const AttachmentPart::List &selectedParts)
{
    d->selectedParts = selectedParts;
}

void AbstractAttachmentControllerBase::exportPublicKey(const QString &fingerprint)
{
    if (fingerprint.isEmpty() || !QGpgME::openpgp()) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Tried to export key with empty fingerprint, or no OpenPGP.";
        return;
    }

    auto ajob = new MessageComposer::AttachmentFromPublicKeyJob(fingerprint, this);
    connect(ajob, &AttachmentFromPublicKeyJob::result, this, [this](KJob *job) {
        d->attachPublicKeyJobResult(job);
    });
    ajob->start();
}

void AbstractAttachmentControllerBase::Private::attachClipBoardElement(KJob *job)
{
    if (job->error()) {
        qCDebug(MESSAGECOMPOSER_LOG) << " Error during when get try to attach text from clipboard";
        Q_EMIT q->errorOccured(job->errorString(), i18n("Failed to attach text from clipboard."));
        return;
    }

    auto ajob = static_cast<MessageComposer::AttachmentClipBoardJob *>(job);
    AttachmentPart::Ptr part = ajob->attachmentPart();
    q->addAttachment(part);
}

static QTemporaryFile *dumpAttachmentToTempFile(const AttachmentPart::Ptr &part) // local
{
    auto file = new QTemporaryFile;
    if (!file->open()) {
        qCCritical(MESSAGECOMPOSER_LOG) << "Could not open tempfile" << file->fileName();
        delete file;
        return nullptr;
    }
    if (file->write(part->data()) == -1) {
        qCCritical(MESSAGECOMPOSER_LOG) << "Could not dump attachment to tempfile.";
        delete file;
        return nullptr;
    }
    file->flush();
    return file;
}

AbstractAttachmentControllerBase::AbstractAttachmentControllerBase(MessageComposer::AttachmentModel *model, QObject *parent)
    : QObject(parent)
    , d(new Private(this))
{
    d->model = model;
    connect(model, &MessageComposer::AttachmentModel::attachUrlsRequested, this, &AbstractAttachmentControllerBase::addAttachments);
    connect(model, &MessageComposer::AttachmentModel::attachmentRemoved, this, [this](const MessageCore::AttachmentPart::Ptr &attr) {
        d->attachmentRemoved(attr);
    });
    connect(model, &AttachmentModel::attachmentCompressRequested, this, &AbstractAttachmentControllerBase::compressAttachment);
    connect(model, &MessageComposer::AttachmentModel::encryptEnabled, this, &AbstractAttachmentControllerBase::setEncryptEnabled);
    connect(model, &MessageComposer::AttachmentModel::signEnabled, this, &AbstractAttachmentControllerBase::setSignEnabled);
}

AbstractAttachmentControllerBase::~AbstractAttachmentControllerBase()
{
    delete d;
}

void MessageComposer::AbstractAttachmentControllerBase::overwriteAttachment(KJob *job)
{
    d->compressJobOverwriteResult(job);
}

void AbstractAttachmentControllerBase::setEncryptEnabled(bool enabled)
{
    if (d->encryptEnabled == enabled) {
        return;
    }
    d->encryptEnabled = enabled;
    Q_EMIT encryptEnabledChanged();
}

void AbstractAttachmentControllerBase::setSignEnabled(bool enabled)
{
    if (d->signEnabled == enabled) {
        d->signEnabled = enabled;
    }
    d->signEnabled = enabled;
    Q_EMIT signEnabledChanged();
}

void AbstractAttachmentControllerBase::compressAttachment(const AttachmentPart::Ptr &part, bool compress)
{
    if (compress) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Compressing part.";

        auto ajob = new AttachmentCompressJob(part, this);
        connect(ajob, &AttachmentCompressJob::result, this, [this](KJob *job) {
            d->compressJobResult(job);
        });
        ajob->start();
    } else {
        qCDebug(MESSAGECOMPOSER_LOG) << "Uncompressing part.";

        // Replace the compressed part with the original uncompressed part, and delete
        // the compressed part.
        AttachmentPart::Ptr originalPart = d->uncompressedParts.take(part);
        Q_ASSERT(originalPart); // Found in uncompressedParts.
        bool ok = d->model->replaceAttachment(part, originalPart);
        Q_ASSERT(ok);
        Q_UNUSED(ok)
    }
}

void AbstractAttachmentControllerBase::slotOpenWithDialog()
{
    openWith();
}

void AbstractAttachmentControllerBase::openWith(const KService::Ptr &offer)
{
    QTemporaryFile *tempFile = dumpAttachmentToTempFile(d->selectedParts.constFirst());
    if (!tempFile) {
        Q_EMIT errorOccured(i18n("KMail was unable to write the attachment to a temporary file."), i18n("Unable to edit attachment"));
        return;
    }
    QUrl url = QUrl::fromLocalFile(tempFile->fileName());
    tempFile->setPermissions(QFile::ReadUser);
    // If offer is null, this will show the "open with" dialog
    auto job = new KIO::ApplicationLauncherJob(offer);
    job->setUrls({url});
    job->start();
    connect(job, &KJob::result, this, [this, tempFile, job]() {
        if (job->error()) {
            Q_EMIT errorOccured(job->errorString(), QString());
            delete tempFile;
        }
    });
    // Delete the file only when the composer is closed
    // (and this object is destroyed).
    tempFile->setParent(this); // Manages lifetime.
}

void AbstractAttachmentControllerBase::openAttachment(const AttachmentPart::Ptr &part)
{
    QTemporaryFile *tempFile = dumpAttachmentToTempFile(part);
    if (!tempFile) {
        Q_EMIT errorOccured(i18n("KMail was unable to write the attachment to a temporary file."), i18n("Unable to open attachment"));
        return;
    }
    tempFile->setPermissions(QFile::ReadUser);
    auto job = new KIO::OpenUrlJob(QUrl::fromLocalFile(tempFile->fileName()), QString::fromLatin1(part->mimeType()));
    job->setDeleteTemporaryFile(true);
    connect(job, &KIO::OpenUrlJob::result, this, [this, tempFile](KJob *job) {
        if (job->error() == KIO::ERR_USER_CANCELED) {
            Q_EMIT errorOccured(i18n("KMail was unable to open the attachment."), job->errorString());
            delete tempFile;
        } else {
            // The file was opened.  Delete it only when the composer is closed
            // (and this object is destroyed).
            tempFile->setParent(this); // Manages lifetime.
        }
    });
    job->start();
}

void AbstractAttachmentControllerBase::viewAttachment(const AttachmentPart::Ptr &part)
{
    auto composer = new MessageComposer::Composer;
    composer->globalPart()->setFallbackCharsetEnabled(true);
    auto attachmentJob = new MessageComposer::AttachmentJob(part, composer);
    connect(attachmentJob, &AttachmentJob::result, this, [this](KJob *job) {
        d->slotAttachmentContentCreated(job);
    });
    attachmentJob->start();
}

void AbstractAttachmentControllerBase::Private::slotAttachmentContentCreated(KJob *job)
{
    if (!job->error()) {
        const MessageComposer::AttachmentJob *const attachmentJob = qobject_cast<MessageComposer::AttachmentJob *>(job);
        Q_ASSERT(attachmentJob);
        if (attachmentJob) {
            Q_EMIT q->showAttachment(attachmentJob->content(), QByteArray());
        }
    } else {
        // TODO: show warning to the user
        qCWarning(MESSAGECOMPOSER_LOG) << "Error creating KMime::Content for attachment:" << job->errorText();
    }
}

void AbstractAttachmentControllerBase::byteArrayToRemoteFile(const QByteArray &aData, const QUrl &aURL, bool overwrite)
{
    KIO::StoredTransferJob *job = KIO::storedPut(aData, aURL, -1, overwrite ? KIO::Overwrite : KIO::DefaultFlags);
    connect(job, &KIO::StoredTransferJob::result, this, &AbstractAttachmentControllerBase::slotPutResult);
}

void AbstractAttachmentControllerBase::slotPutResult(KJob *job)
{
    auto _job = qobject_cast<KIO::StoredTransferJob *>(job);

    if (job->error()) {
        if (job->error() == KIO::ERR_FILE_ALREADY_EXIST) {
            if (KMessageBox::warningContinueCancel(nullptr,
                                                   i18n("File %1 exists.\nDo you want to replace it?", _job->url().toLocalFile()),
                                                   i18n("Save to File"),
                                                   KGuiItem(i18n("&Replace")))
                == KMessageBox::Continue) {
                byteArrayToRemoteFile(_job->data(), _job->url(), true);
            }
        } else {
            KJobUiDelegate *ui = static_cast<KIO::Job *>(job)->uiDelegate();
            ui->showErrorMessage();
        }
    }
}

void AbstractAttachmentControllerBase::showAttachClipBoard()
{
    auto job = new MessageComposer::AttachmentClipBoardJob(this);
    connect(job, &AttachmentClipBoardJob::result, this, [this](KJob *job) {
        d->attachClipBoardElement(job);
    });
    job->start();
}

void AbstractAttachmentControllerBase::addAttachment(const AttachmentPart::Ptr &part)
{
    part->setEncrypted(d->model->isEncryptSelected());
    part->setSigned(d->model->isSignSelected());
    d->model->addAttachment(part);

    Q_EMIT fileAttached();
}

void AbstractAttachmentControllerBase::addAttachmentUrlSync(const QUrl &url)
{
    MessageCore::AttachmentFromUrlBaseJob *ajob = MessageCore::AttachmentFromUrlUtils::createAttachmentJob(url, this);
    if (ajob->exec()) {
        AttachmentPart::Ptr part = ajob->attachmentPart();
        addAttachment(part);
    } else {
        if (ajob->error()) {
            Q_EMIT errorOccured(ajob->errorString(), i18nc("@title:window", "Failed to attach file"));
        }
    }
}

void AbstractAttachmentControllerBase::addAttachment(const QUrl &url)
{
    MessageCore::AttachmentFromUrlBaseJob *ajob = MessageCore::AttachmentFromUrlUtils::createAttachmentJob(url, this);
    connect(ajob, &AttachmentFromUrlBaseJob::result, this, [this](KJob *job) {
        d->loadJobResult(job);
    });
    ajob->start();
}

void AbstractAttachmentControllerBase::addAttachments(const QList<QUrl> &urls)
{
    for (const QUrl &url : urls) {
        addAttachment(url);
    }
}

void AbstractAttachmentControllerBase::attachMyPublicKey()
{
}

void AbstractAttachmentControllerBase::enableAttachPublicKey(bool enable)
{
    if (d->canAttachPublicKey == enable) {
        return;
    }
    d->canAttachPublicKey = enable;
    Q_EMIT canAttachPublicKeyChanged();
}

void AbstractAttachmentControllerBase::enableAttachMyPublicKey(bool enable)
{
    if (d->canAttachOwnPublicKey == enable) {
        return;
    }
    d->canAttachOwnPublicKey = enable;
    Q_EMIT canAttachOwnPublicKeyChanged();
}

void AbstractAttachmentControllerBase::setAttachOwnVcard(bool attachVcard)
{
    if (d->attachOwnVCard == attachVcard) {
        return;
    }
    d->attachOwnVCard = attachVcard;
    Q_EMIT attachOwnVcardChanged();
}

bool AbstractAttachmentControllerBase::attachOwnVcard() const
{
    return d->attachOwnVCard;
}

void AbstractAttachmentControllerBase::setIdentityHasOwnVcard(bool state)
{
    if (d->identityHasOwnVcard == state) {
        return;
    }
    d->identityHasOwnVcard = state;
    Q_EMIT identityHasOwnVcardChanged();
}

bool AbstractAttachmentControllerBase::canAttachOwnPublicKey() const
{
    return d->canAttachOwnPublicKey;
}

bool AbstractAttachmentControllerBase::canAttachPublicKey() const
{
    return d->canAttachPublicKey;
}

bool AbstractAttachmentControllerBase::encryptEnabled() const
{
    return d->encryptEnabled;
}

bool AbstractAttachmentControllerBase::identityHasOwnVcard() const
{
    return d->identityHasOwnVcard;
}

bool MessageComposer::AbstractAttachmentControllerBase::signEnabled() const
{
    return d->signEnabled;
}

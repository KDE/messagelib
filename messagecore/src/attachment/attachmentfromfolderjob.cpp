/*
    SPDX-FileCopyrightText: 2011 Martin Bednár <serafean@gmail.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "attachmentfromfolderjob.h"

#include "messagecore_debug.h"
#include <KLocalizedString>

#include <QBuffer>
#include <QDir>
#include <QScopedPointer>
#include <QSharedPointer>

static const mode_t archivePermsAttachment = S_IFREG | 0644;

using namespace MessageCore;

class Q_DECL_HIDDEN AttachmentFromFolderJob::Private
{
public:
    Private(AttachmentFromFolderJob *qq);

    void compressFolder();
    void addEntity(const QFileInfoList &f, const QString &path);

    AttachmentFromFolderJob *const q;
    KZip::Compression mCompression = KZip::DeflateCompression;
    AttachmentPart::Ptr mCompressedFolder;
    QScopedPointer<KZip> mZip;
    QDateTime mArchiveTime = QDateTime::currentDateTime();
};

AttachmentFromFolderJob::Private::Private(AttachmentFromFolderJob *qq)
    : q(qq)
    , mZip(nullptr)
{
}

void AttachmentFromFolderJob::Private::compressFolder()
{
    qCDebug(MESSAGECORE_LOG) << "starting compression";
    QString fileName = q->url().fileName();
    QByteArray array;
    QBuffer dev(&array);
    mZip.reset(new KZip(&dev));
    if (!mZip->open(QIODevice::WriteOnly)) {
        q->setError(KJob::UserDefinedError);
        q->setErrorText(i18n("Could not create compressed file."));
        q->emitResult();
        return;
    }
    mZip->setCompression(mCompression);
    const QString filename = q->url().fileName();
    if (!mZip->writeDir(filename, QString(), QString(), 040755, mArchiveTime, mArchiveTime, mArchiveTime)) {
        qCWarning(MESSAGECORE_LOG) << " Impossible to write file " << fileName;
    }
    qCDebug(MESSAGECORE_LOG) << "writing root directory : " << filename;
    addEntity(QDir(q->url().path()).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files, QDir::DirsFirst),
              fileName + QLatin1Char('/'));
    mZip->close();

    Q_ASSERT(mCompressedFolder == nullptr);

    mCompressedFolder = AttachmentPart::Ptr(new AttachmentPart);
    const QString newName = fileName + QLatin1String(".zip");
    mCompressedFolder->setName(newName);
    mCompressedFolder->setFileName(newName);
    mCompressedFolder->setMimeType("application/zip");
    mCompressedFolder->setUrl(q->url());
    //     mCompressedFolder->setCompressed( true );
    mCompressedFolder->setData(array);
    //     mCompressedFolder->setCompressible(false);
    q->setAttachmentPart(mCompressedFolder);
    q->emitResult();

    // TODO:add allowCompression bool to AttachmentPart && modify GUI to disable decompressing.
    //  Or leave attachment as uncompressed and let it be compressed again?
}

void AttachmentFromFolderJob::Private::addEntity(const QFileInfoList &f, const QString &path)
{
    for (const QFileInfo &info : f) {
        qCDebug(MESSAGECORE_LOG) << q->maximumAllowedSize() << "Attachment size : " << mZip->device()->size();

        if (q->maximumAllowedSize() != -1 && mZip->device()->size() > q->maximumAllowedSize()) {
            q->setError(KJob::UserDefinedError);
            q->setErrorText(i18n("The resulting attachment would be larger than the maximum allowed size, aborting."));
            q->emitResult();
            return;
        }

        const QString infoFileName = info.fileName();
        if (info.isDir()) {
            qCDebug(MESSAGECORE_LOG) << "adding directory " << infoFileName << "to zip";
            if (!mZip->writeDir(path + infoFileName, QString(), QString(), 040755, mArchiveTime, mArchiveTime, mArchiveTime)) {
                q->setError(KJob::UserDefinedError);
                q->setErrorText(i18n("Could not add %1 to the archive", infoFileName));
                q->emitResult();
                return;
            }
            addEntity(QDir(info.filePath()).entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::Files, QDir::DirsFirst),
                      path + infoFileName + QLatin1Char('/'));
        }

        if (info.isFile()) {
            qCDebug(MESSAGECORE_LOG) << "Adding file " << path + infoFileName << "to zip";
            QFile file(info.filePath());
            if (!file.open(QIODevice::ReadOnly)) {
                q->setError(KJob::UserDefinedError);
                q->setErrorText(i18n("Could not open %1 for reading.", file.fileName()));
                q->emitResult();
                return;
            }
            if (!mZip->writeFile(path + infoFileName, file.readAll(), archivePermsAttachment, QString(), QString(), mArchiveTime, mArchiveTime, mArchiveTime)) {
                q->setError(KJob::UserDefinedError);
                q->setErrorText(i18n("Could not add %1 to the archive", file.fileName()));
                q->emitResult();
            }
            file.close();
        }
    }
}

AttachmentFromFolderJob::AttachmentFromFolderJob(const QUrl &url, QObject *parent)
    : AttachmentFromUrlBaseJob(url, parent)
    , d(new Private(this))
{
}

AttachmentFromFolderJob::~AttachmentFromFolderJob()
{
    delete d;
}

void AttachmentFromFolderJob::setCompression(KZip::Compression compression)
{
    d->mCompression = compression;
}

KZip::Compression AttachmentFromFolderJob::compression() const
{
    return d->mCompression;
}

void AttachmentFromFolderJob::doStart()
{
    d->compressFolder();
}

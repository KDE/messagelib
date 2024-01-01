/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "attachmenttemporaryfilesdirs.h"

#include <QDir>
#include <QFile>
#include <QTimer>

using namespace MimeTreeParser;

class MimeTreeParser::AttachmentTemporaryFilesDirsPrivate
{
public:
    AttachmentTemporaryFilesDirsPrivate() = default;

    QStringList mTempFiles;
    QStringList mTempDirs;
    int mDelayRemoveAll = 10000;
};

AttachmentTemporaryFilesDirs::AttachmentTemporaryFilesDirs(QObject *parent)
    : QObject(parent)
    , d(new AttachmentTemporaryFilesDirsPrivate)
{
}

AttachmentTemporaryFilesDirs::~AttachmentTemporaryFilesDirs() = default;

void AttachmentTemporaryFilesDirs::setDelayRemoveAllInMs(int ms)
{
    d->mDelayRemoveAll = (ms < 0) ? 0 : ms;
}

void AttachmentTemporaryFilesDirs::removeTempFiles()
{
    QTimer::singleShot(d->mDelayRemoveAll, this, &AttachmentTemporaryFilesDirs::slotRemoveTempFiles);
}

void AttachmentTemporaryFilesDirs::forceCleanTempFiles()
{
    QStringList::ConstIterator end = d->mTempFiles.constEnd();
    for (QStringList::ConstIterator it = d->mTempFiles.constBegin(); it != end; ++it) {
        QFile::remove(*it);
    }
    d->mTempFiles.clear();
    end = d->mTempDirs.constEnd();
    for (QStringList::ConstIterator it = d->mTempDirs.constBegin(); it != end; ++it) {
        QDir(*it).rmdir(*it);
    }
    d->mTempDirs.clear();
}

void AttachmentTemporaryFilesDirs::slotRemoveTempFiles()
{
    forceCleanTempFiles();
    // Delete it after cleaning
    deleteLater();
}

void AttachmentTemporaryFilesDirs::addTempFile(const QString &file)
{
    if (!d->mTempFiles.contains(file)) {
        d->mTempFiles.append(file);
    }
}

void AttachmentTemporaryFilesDirs::addTempDir(const QString &dir)
{
    if (!d->mTempDirs.contains(dir)) {
        d->mTempDirs.append(dir);
    }
}

QStringList AttachmentTemporaryFilesDirs::temporaryFiles() const
{
    return d->mTempFiles;
}

QStringList AttachmentTemporaryFilesDirs::temporaryDirs() const
{
    return d->mTempDirs;
}

#include "moc_attachmenttemporaryfilesdirs.cpp"

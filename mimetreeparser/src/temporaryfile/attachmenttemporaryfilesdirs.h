/*
  SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef ATTACHMENTTEMPORARYFILESDIRS_H
#define ATTACHMENTTEMPORARYFILESDIRS_H

#include <QObject>
#include <QStringList>
#include "mimetreeparser_export.h"

namespace MimeTreeParser {
class AttachmentTemporaryFilesDirsPrivate;
/**
 * @brief The AttachmentTemporaryFilesDirs class
 * @author Laurent Montel <montel@kde.org>
 */
class MIMETREEPARSER_EXPORT AttachmentTemporaryFilesDirs : public QObject
{
    Q_OBJECT
public:
    explicit AttachmentTemporaryFilesDirs(QObject *parent = nullptr);
    ~AttachmentTemporaryFilesDirs() override;

    void addTempFile(const QString &file);
    void addTempDir(const QString &dir);
    Q_REQUIRED_RESULT QStringList temporaryFiles() const;
    void removeTempFiles();
    void forceCleanTempFiles();

    Q_REQUIRED_RESULT QStringList temporaryDirs() const;

    void setDelayRemoveAllInMs(int ms);

private Q_SLOTS:
    void slotRemoveTempFiles();

private:
    AttachmentTemporaryFilesDirsPrivate *const d;
};
}

#endif // ATTACHMENTTEMPORARYFILESDIRS_H

/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser_export.h"
#include <QObject>
#include <QStringList>
#include <memory>
namespace MimeTreeParser
{
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
    [[nodiscard]] QStringList temporaryFiles() const;
    void removeTempFiles();
    void forceCleanTempFiles();

    [[nodiscard]] QStringList temporaryDirs() const;

    void setDelayRemoveAllInMs(int ms);

private:
    MIMETREEPARSER_NO_EXPORT void slotRemoveTempFiles();
    std::unique_ptr<AttachmentTemporaryFilesDirsPrivate> const d;
};
}

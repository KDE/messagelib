/*
  SPDX-FileCopyrightText: 2013-2026 Laurent Montel <montel@kde.org>

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
 * \class MimeTreeParser::AttachmentTemporaryFilesDirs
 * \inmodule MimeTreeParser
 * \inheaderfile MimeTreeParser/AttachmentTemporaryFilesDirs
 *
 * \brief The AttachmentTemporaryFilesDirs class
 * \author Laurent Montel <montel@kde.org>
 */
class MIMETREEPARSER_EXPORT AttachmentTemporaryFilesDirs : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Constructor.
     * \param parent The parent object.
     */
    explicit AttachmentTemporaryFilesDirs(QObject *parent = nullptr);
    /*!
     * \brief Destructor.
     */
    ~AttachmentTemporaryFilesDirs() override;

    /*!
     * \brief Add a temporary file.
     * \param file The file path to add.
     */
    void addTempFile(const QString &file);
    /*!
     * \brief Add a temporary directory.
     * \param dir The directory path to add.
     */
    void addTempDir(const QString &dir);
    /*!
     * \brief Get temporary files.
     * \return List of temporary file paths.
     */
    [[nodiscard]] QStringList temporaryFiles() const;
    /*!
     * \brief Remove temporary files.
     */
    void removeTempFiles();
    /*!
     * \brief Force clean temporary files.
     */
    void forceCleanTempFiles();

    /*!
     * \brief Get temporary directories.
     * \return List of temporary directory paths.
     */
    [[nodiscard]] QStringList temporaryDirs() const;
    /*!
     */
    void setDelayRemoveAllInMs(int ms);

private:
    MIMETREEPARSER_NO_EXPORT void slotRemoveTempFiles();
    std::unique_ptr<AttachmentTemporaryFilesDirsPrivate> const d;
};
}

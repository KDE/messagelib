/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class AttachmentTemporaryFilesDirsTest : public QObject
{
    Q_OBJECT
public:
    explicit AttachmentTemporaryFilesDirsTest(QObject *parent = nullptr);
    ~AttachmentTemporaryFilesDirsTest() override;
private Q_SLOTS:
    void shouldHaveDefaultValue();
    void shouldAddTemporaryFiles();
    void shouldAddTemporaryDirs();
    void shouldNotAddSameFiles();
    void shouldNotAddSameDirs();
    void shouldForceRemoveTemporaryDirs();
    void shouldForceRemoveTemporaryFiles();
    void shouldCreateDeleteTemporaryFiles();
    void shouldRemoveTemporaryFilesAfterTime();
};

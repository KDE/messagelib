/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>

namespace MessageComposer
{
/**
 * @brief The ComposerAttachmentInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT ComposerAttachmentInterface
{
public:
    ComposerAttachmentInterface();
    ~ComposerAttachmentInterface();

    [[nodiscard]] QStringList fileNames() const;
    void setFileNames(const QStringList &fileName);

    [[nodiscard]] int count() const;
    void setCount(int count);

    [[nodiscard]] QStringList namesAndSize() const;
    void setNamesAndSize(const QStringList &nameAndSize);

    [[nodiscard]] QStringList names() const;
    void setNames(const QStringList &name);

private:
    QStringList mFileNames;
    QStringList mNamesAndSize;
    QStringList mNames;
    int mCount = 0;
};
}

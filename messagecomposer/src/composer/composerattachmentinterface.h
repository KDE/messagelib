/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/


#ifndef COMPOSERATTACHMENTINTERFACE_H
#define COMPOSERATTACHMENTINTERFACE_H

#include "messagecomposer_export.h"
#include <QObject>

namespace MessageComposer {
class ComposerViewBase;
/**
 * @brief The ComposerAttachmentInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT ComposerAttachmentInterface
{
public:
    ComposerAttachmentInterface();
    ~ComposerAttachmentInterface();

    Q_REQUIRED_RESULT QStringList fileNames() const;
    void setFileNames(const QStringList &fileName);

    Q_REQUIRED_RESULT int count() const;
    void setCount(int count);

    Q_REQUIRED_RESULT QStringList namesAndSize() const;
    void setNamesAndSize(const QStringList &nameAndSize);

    Q_REQUIRED_RESULT QStringList names() const;
    void setNames(const QStringList &name);

private:
    QStringList mFileNames;
    QStringList mNamesAndSize;
    QStringList mNames;
    int mCount = 0;
};
}
#endif // COMPOSERATTACHMENTINTERFACE_H

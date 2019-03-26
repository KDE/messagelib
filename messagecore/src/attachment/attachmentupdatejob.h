/*
   Copyright (C) 2014-2019 Laurent Montel <montel@kde.org>

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

#ifndef ATTACHMENTUPDATEJOB_H
#define ATTACHMENTUPDATEJOB_H

#include "messagecore_private_export.h"
#include <KJob>
#include "attachmentpart.h"

namespace MessageCore {
class MESSAGECORE_TESTS_EXPORT AttachmentUpdateJob : public KJob
{
    Q_OBJECT
public:
    explicit AttachmentUpdateJob(const AttachmentPart::Ptr &part, QObject *parent = nullptr);
    ~AttachmentUpdateJob() override;

    void start() override;
    Q_REQUIRED_RESULT AttachmentPart::Ptr originalPart() const;
    Q_REQUIRED_RESULT AttachmentPart::Ptr updatedPart() const;
private:
    //@cond PRIVATE
    class Private;
    Private *const d;
};
}

#endif // ATTACHMENTUPDATEJOB_H

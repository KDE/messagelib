/*
  Copyright (c) 2009 Constantin Berzan <exit3219@gmail.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License as
  published by the Free Software Foundation; either version 2 of
  the License or (at your option) version 3 or any later version
  accepted by the membership of KDE e.V. (or its successor approved
  by the membership of KDE e.V.), which shall act as a proxy
  defined in Section 14 of version 3 of the license.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef MESSAGE_ATTACHMENTFROMPUBLICKEYJOB_H
#define MESSAGE_ATTACHMENTFROMPUBLICKEYJOB_H

#include <MessageCore/AttachmentLoadJob>

#include "messagecomposer_export.h"

namespace MessageComposer {
// TODO I have no idea how to test this.  Have a fake keyring???
/**
 * @brief The AttachmentFromPublicKeyJob class
 */
class MESSAGECOMPOSER_EXPORT AttachmentFromPublicKeyJob : public MessageCore::AttachmentLoadJob
{
    Q_OBJECT

public:
    explicit AttachmentFromPublicKeyJob(const QString &fingerprint, QObject *parent = nullptr);
    ~AttachmentFromPublicKeyJob() override;

    Q_REQUIRED_RESULT QString fingerprint() const;
    void setFingerprint(const QString &fingerprint);

protected Q_SLOTS:
    void doStart() override;

private:
    class Private;
    friend class Private;
    Private *const d;
};
} //

#endif

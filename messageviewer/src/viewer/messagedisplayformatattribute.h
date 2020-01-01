/*
  Copyright (c) 2013-2020 Laurent Montel <montel@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

*/

#ifndef MESSAGEDISPLAYFORMATATTRIBUTE_H
#define MESSAGEDISPLAYFORMATATTRIBUTE_H

#include <AkonadiCore/attribute.h>
#include "messageviewer_export.h"
#include "messageviewer/viewer.h"
namespace MessageViewer {
class MessageDisplayFormatAttributePrivate;
/**
 * @brief The MessageDisplayFormatAttribute class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MessageDisplayFormatAttribute : public Akonadi::Attribute
{
public:
    explicit MessageDisplayFormatAttribute();
    ~MessageDisplayFormatAttribute() override;

    Q_REQUIRED_RESULT MessageDisplayFormatAttribute *clone() const override;
    Q_REQUIRED_RESULT QByteArray type() const override;
    Q_REQUIRED_RESULT QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;

    void setMessageFormat(Viewer::DisplayFormatMessage format);
    Q_REQUIRED_RESULT Viewer::DisplayFormatMessage messageFormat() const;

    void setRemoteContent(bool remote);
    Q_REQUIRED_RESULT bool remoteContent() const;

    Q_REQUIRED_RESULT bool operator==(const MessageDisplayFormatAttribute &other) const;

private:
    friend class MessageDisplayFormatAttributePrivate;
    MessageDisplayFormatAttributePrivate *const d;
};
}

#endif // MESSAGEDISPLAYFORMATATTRIBUTE_H

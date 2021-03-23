/*
  SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messageviewer/viewer.h"
#include "messageviewer_export.h"
#include <AkonadiCore/attribute.h>
namespace MessageViewer
{
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


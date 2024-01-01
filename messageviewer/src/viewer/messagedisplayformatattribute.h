/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#pragma once

#include "messageviewer/viewer.h"
#include "messageviewer_export.h"
#include <Akonadi/Attribute>
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

    [[nodiscard]] MessageDisplayFormatAttribute *clone() const override;
    [[nodiscard]] QByteArray type() const override;
    [[nodiscard]] QByteArray serialized() const override;
    void deserialize(const QByteArray &data) override;

    void setMessageFormat(Viewer::DisplayFormatMessage format);
    [[nodiscard]] Viewer::DisplayFormatMessage messageFormat() const;

    void setRemoteContent(bool remote);
    [[nodiscard]] bool remoteContent() const;

    [[nodiscard]] bool operator==(const MessageDisplayFormatAttribute &other) const;

private:
    friend class MessageDisplayFormatAttributePrivate;
    std::unique_ptr<MessageDisplayFormatAttributePrivate> const d;
};
}

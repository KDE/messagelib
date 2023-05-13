/*
  SPDX-FileCopyrightText: 2023 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QString>
#include <memory>

#include "messagecomposer_export.h"
#include "messagepart.h"

namespace MessageComposer
{
/**
 * @brief The ItipPart class
 */
class MESSAGECOMPOSER_EXPORT ItipPart : public MessagePart
{
    Q_OBJECT
    Q_PROPERTY(QString invitation READ invitation WRITE setInvitation NOTIFY invitationChanged)
    Q_PROPERTY(QString invitationBody READ invitationBody WRITE setInvitationBody NOTIFY invitationBodyChanged)
    Q_PROPERTY(bool outlookConformInvitation READ outlookConformInvitation WRITE setOutlookConformInvitation NOTIFY outlookConformInvitationChanged)

public:
    explicit ItipPart(QObject *parent = nullptr);
    ~ItipPart() override;

    Q_REQUIRED_RESULT QString invitation() const;
    void setInvitation(const QString &invitation);

    Q_REQUIRED_RESULT QString invitationBody() const;
    void setInvitationBody(const QString &invitationBody);

    // default false
    Q_REQUIRED_RESULT bool outlookConformInvitation() const;
    void setOutlookConformInvitation(bool enabled);

Q_SIGNALS:
    void invitationChanged();
    void invitationBodyChanged();
    void outlookConformInvitationChanged();

private:
    class ItipPartPrivate;
    std::unique_ptr<ItipPartPrivate> const d;
};
}

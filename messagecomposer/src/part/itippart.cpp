/*
  SPDX-FileCopyrightText: 2023 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "itippart.h"

using namespace MessageComposer;

class ItipPart::ItipPartPrivate
{
public:
    QString invitation;
    QString invitationBody;
    bool outlookComnformInvitation = false;
};

ItipPart::ItipPart(QObject *parent)
    : MessagePart(parent)
    , d(std::make_unique<ItipPartPrivate>())
{
}

ItipPart::~ItipPart() = default;

bool ItipPart::outlookConformInvitation() const
{
    return d->outlookComnformInvitation;
}

void ItipPart::setOutlookConformInvitation(bool enabled)
{
    if (d->outlookComnformInvitation == enabled) {
        return;
    }

    d->outlookComnformInvitation = enabled;
    Q_EMIT outlookConformInvitationChanged();
}

QString ItipPart::invitation() const
{
    return d->invitation;
}

void ItipPart::setInvitation(const QString &invitation)
{
    if (d->invitation == invitation) {
        return;
    }

    d->invitation = invitation;
    Q_EMIT invitationChanged();
}

QString ItipPart::invitationBody() const
{
    return d->invitationBody;
}

void ItipPart::setInvitationBody(const QString &invitationBody)
{
    if (d->invitationBody == invitationBody) {
        return;
    }

    d->invitationBody = invitationBody;
    Q_EMIT invitationBodyChanged();
}

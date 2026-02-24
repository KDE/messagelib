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
/*!
 * \class MessageComposer::ItipPart
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/ItipPart
 * \brief The ItipPart class
 */
class MESSAGECOMPOSER_EXPORT ItipPart : public MessagePart
{
    Q_OBJECT
    Q_PROPERTY(QString invitation READ invitation WRITE setInvitation NOTIFY invitationChanged)
    Q_PROPERTY(QString invitationBody READ invitationBody WRITE setInvitationBody NOTIFY invitationBodyChanged)
    Q_PROPERTY(bool outlookConformInvitation READ outlookConformInvitation WRITE setOutlookConformInvitation NOTIFY outlookConformInvitationChanged)
    Q_PROPERTY(QString method READ method WRITE setMethod NOTIFY methodChanged)

public:
    /*! \brief Constructs an ItipPart.
        \param parent The parent object.
    */
    explicit ItipPart(QObject *parent = nullptr);
    /*! \brief Destroys the ItipPart. */
    ~ItipPart() override;

    /*! \brief Returns the iCalendar method (PUBLISH, REQUEST, REPLY, etc.). */
    [[nodiscard]] QString method() const;
    /*! \brief Sets the iCalendar method.
        \param method The method string (e.g., "PUBLISH", "REQUEST").
    */
    void setMethod(const QString &method);

    /*! \brief Returns the invitation iCalendar data. */
    [[nodiscard]] QString invitation() const;
    /*! \brief Sets the invitation iCalendar data.
        \param invitation The iCalendar event data.
    */
    void setInvitation(const QString &invitation);

    /*! \brief Returns the plain text body of the invitation. */
    [[nodiscard]] QString invitationBody() const;
    /*! \brief Sets the plain text body of the invitation.
        \param invitationBody The invitation body text.
    */
    void setInvitationBody(const QString &invitationBody);

    // default false
    /*! \brief Returns whether the invitation should conform to Outlook format. */
    [[nodiscard]] bool outlookConformInvitation() const;
    /*! \brief Sets whether the invitation should conform to Outlook format.
        \param enabled True to use Outlook-compatible format.
    */
    void setOutlookConformInvitation(bool enabled);

Q_SIGNALS:
    /*! \brief Emitted when the iCalendar method changes. */
    void methodChanged();
    /*! \brief Emitted when the invitation iCalendar data changes. */
    void invitationChanged();
    /*! \brief Emitted when the invitation body text changes. */
    void invitationBodyChanged();
    /*! \brief Emitted when the Outlook conformance setting changes. */
    void outlookConformInvitationChanged();

private:
    class ItipPartPrivate;
    std::unique_ptr<ItipPartPrivate> const d;
};
}

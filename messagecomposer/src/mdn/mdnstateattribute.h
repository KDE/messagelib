/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>
 *  SPDX-FileCopyrightText: 2010 KDAB
 *
 *  SPDX-License-Identifier: LGPL-2.0-or-later
 *
 *****************************************************************************/

#pragma once

#include "mailcommon_export.h"

#include <Akonadi/Attribute>
#include <memory>

namespace MailCommon
{
/**
 * @short An Attribute that keeps track of the MDN state of a mail message.
 *
 * Once a mail that contains a Message Disposition Notification is processed,
 * the outcome of the user action will be stored in this attribute.
 *
 * @author Leo Franchi <lfranchi@kde.org>
 * @see Akonadi::Attribute
 * @since 4.6
 */
class MAILCOMMON_EXPORT MDNStateAttribute : public Akonadi::Attribute
{
public:
    /**
     * Describes the "MDN sent" state.
     */
    enum MDNSentState {
        MDNStateUnknown, ///< The state is unknown.
        MDNNone, ///< No MDN has been set.
        MDNIgnore, ///< Ignore sending a MDN.
        MDNDisplayed, ///< The message has been displayed by the UA to someone reading the recipient's mailbox.
        MDNDeleted, ///< The message has been deleted.
        MDNDispatched, ///< The message has been sent somewhere in some manner without necessarily having been previously displayed to the user.
        MDNProcessed, ///< The message has been processed in some manner without being displayed to the user.
        MDNDenied, ///< The recipient does not wish the sender to be informed of the message's disposition.
        MDNFailed ///< A failure occurred that prevented the proper generation of an MDN.
    };

    /**
     * Creates a new MDN state attribute.
     *
     * @param state The state the attribute will have.
     */
    explicit MDNStateAttribute(MDNSentState state = MDNStateUnknown);

    /**
     * Creates a new MDN state attribute.
     *
     * @param state The string representation of the state the attribute will have.
     */
    explicit MDNStateAttribute(const QByteArray &state);

    /**
     * Destroys the MDN state attribute.
     */
    ~MDNStateAttribute() override;

    /**
     * Reimplemented from Attribute
     */
    QByteArray type() const override;

    /**
     * Reimplemented from Attribute
     */
    MDNStateAttribute *clone() const override;

    /**
     * Reimplemented from Attribute
     */
    QByteArray serialized() const override;

    /**
     * Reimplemented from Attribute
     */
    void deserialize(const QByteArray &data) override;

    /**
     * Sets the MDN @p state.
     */
    void setMDNState(MDNSentState state);

    /**
     * Returns the MDN state.
     */
    MDNStateAttribute::MDNSentState mdnState() const;

    bool operator==(const MDNStateAttribute &other) const;

private:
    //@cond PRIVATE
    class MDNStateAttributePrivate;
    std::unique_ptr<MDNStateAttributePrivate> const d;
    //@endcond
};
}

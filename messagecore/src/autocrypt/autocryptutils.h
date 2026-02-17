/*
   SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "autocryptrecipient.h"

#include "messagecore_export.h"

#include <KMime/Headers>
#include <MimeTreeParser/MessagePart>
#include <MimeTreeParser/NodeHelper>

namespace MessageCore
{
/*!
 * \class MessageCore::HeaderMixupNodeHelper
 * \inmodule MessageCore
 * \inheaderfile MessageCore/HeaderMixupNodeHelper
 */
class MESSAGECORE_EXPORT HeaderMixupNodeHelper
{
public:
    /*!
     * Creates a new HeaderMixupNodeHelper instance.
     *
     * \a n The node helper to use.
     * \a m The KMime::Content to work with.
     */
    HeaderMixupNodeHelper(MimeTreeParser::NodeHelper *n, KMime::Content *m);

    /*!
     * Checks if a mail header exists.
     *
     * \a header The header name to check.
     */
    [[nodiscard]] bool hasMailHeader(const char *header) const;
    /*!
     * Returns the mail header as a base KMime header.
     *
     * \a header The header name to retrieve.
     */
    [[nodiscard]] KMime::Headers::Base const *mailHeaderAsBase(const char *header) const;
    /*!
     * Returns the mail header as an address list.
     *
     * \a header The header name to retrieve.
     */
    [[nodiscard]] QSharedPointer<KMime::Headers::Generics::AddressList> mailHeaderAsAddressList(const char *header) const;
    /*!
     * Returns the date header from the message.
     */
    [[nodiscard]] QDateTime dateHeader() const;
    /*!
     * Returns the message parts for the given mail header.
     *
     * \a header The header name to retrieve message parts for.
     */
    [[nodiscard]] QList<MimeTreeParser::MessagePart::Ptr> messagePartsOfMailHeader(const char *header) const;
    /*!
     * Returns all headers of the given type.
     *
     * \a headerType The type of headers to retrieve.
     */
    [[nodiscard]] QList<KMime::Headers::Base *> headers(const char *headerType) const;

public:
    KMime::Content *message = nullptr;

private:
    MimeTreeParser::NodeHelper *const mNodeHelper;
};

[[nodiscard]] QHash<QByteArray, QByteArray> MESSAGECORE_EXPORT paramsFromAutocryptHeader(const KMime::Headers::Base *const header);
/*! Processes autocrypt information from mail. */
MESSAGECORE_EXPORT void processAutocryptfromMail(const HeaderMixupNodeHelper &mixup);
}

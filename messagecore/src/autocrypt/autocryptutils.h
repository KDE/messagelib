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
class MESSAGECORE_EXPORT HeaderMixupNodeHelper
{
public:
    HeaderMixupNodeHelper(MimeTreeParser::NodeHelper *n, KMime::Content *m);

    [[nodiscard]] bool hasMailHeader(const char *header) const;
    [[nodiscard]] KMime::Headers::Base const *mailHeaderAsBase(const char *header) const;
    [[nodiscard]] QSharedPointer<KMime::Headers::Generics::AddressList> mailHeaderAsAddressList(const char *header) const;
    [[nodiscard]] QDateTime dateHeader() const;
    [[nodiscard]] QList<MimeTreeParser::MessagePart::Ptr> messagePartsOfMailHeader(const char *header) const;
    [[nodiscard]] QList<KMime::Headers::Base *> headers(const char *headerType) const;

public:
    KMime::Content *message = nullptr;

private:
    MimeTreeParser::NodeHelper *const mNodeHelper;
};

[[nodiscard]] QHash<QByteArray, QByteArray> MESSAGECORE_EXPORT paramsFromAutocryptHeader(const KMime::Headers::Base *const header);
MESSAGECORE_EXPORT void processAutocryptfromMail(const HeaderMixupNodeHelper &mixup);
}

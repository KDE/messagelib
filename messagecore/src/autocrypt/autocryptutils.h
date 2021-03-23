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

    Q_REQUIRED_RESULT bool hasMailHeader(const char *header) const;
    Q_REQUIRED_RESULT KMime::Headers::Base const *mailHeaderAsBase(const char *header) const;
    Q_REQUIRED_RESULT QSharedPointer<KMime::Headers::Generics::AddressList> mailHeaderAsAddressList(const char *header) const;
    Q_REQUIRED_RESULT QDateTime dateHeader() const;
    Q_REQUIRED_RESULT QVector<MimeTreeParser::MessagePart::Ptr> messagePartsOfMailHeader(const char *header) const;
    Q_REQUIRED_RESULT QVector<KMime::Headers::Base *> headers(const char *headerType) const;

public:
    KMime::Content *message = nullptr;

private:
    MimeTreeParser::NodeHelper *const mNodeHelper;
};

Q_REQUIRED_RESULT QHash<QByteArray, QByteArray> MESSAGECORE_EXPORT paramsFromAutocryptHeader(const KMime::Headers::Base *const header);
MESSAGECORE_EXPORT void processAutocryptfromMail(const HeaderMixupNodeHelper &mixup);
}


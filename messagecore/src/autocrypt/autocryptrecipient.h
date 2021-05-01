/*
  SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"

#include <KMime/Headers>

#include <gpgme++/data.h>

#include <QJsonDocument>

namespace MessageCore
{
class AutocryptRecipientPrivate;
class AutocryptStorage;
class HeaderMixupNodeHelper;

class MESSAGECORE_EXPORT AutocryptRecipient
{
public:
    using Ptr = QSharedPointer<AutocryptRecipient>;
    AutocryptRecipient();
    void updateFromMessage(const HeaderMixupNodeHelper &mixup, const KMime::Headers::Base *header);
    void updateFromGossip(const HeaderMixupNodeHelper &mixup, const KMime::Headers::Base *header);

    Q_REQUIRED_RESULT QByteArray toJson(QJsonDocument::JsonFormat format) const;
    void fromJson(const QByteArray &json);

    Q_REQUIRED_RESULT bool hasChanged() const;
    void setChangedFlag(bool changed);

    Q_REQUIRED_RESULT QByteArray addr() const;
    Q_REQUIRED_RESULT QByteArray bad_user_agent() const;

    Q_REQUIRED_RESULT QDateTime last_seen() const;
    Q_REQUIRED_RESULT QDateTime autocrypt_timestamp() const;
    Q_REQUIRED_RESULT QDateTime counting_since() const;
    Q_REQUIRED_RESULT QDateTime gossip_timestamp() const;

    Q_REQUIRED_RESULT int count_have_ach() const;
    Q_REQUIRED_RESULT int count_no_ach() const;

    Q_REQUIRED_RESULT bool prefer_encrypt() const;

    Q_REQUIRED_RESULT GpgME::Key gpgKey() const;
    Q_REQUIRED_RESULT QByteArray gpgKeydata() const;

    Q_REQUIRED_RESULT GpgME::Key gossipKey() const;
    Q_REQUIRED_RESULT QByteArray gossipKeydata() const;

private:
    std::unique_ptr<AutocryptRecipientPrivate> d_ptr;
    Q_DECLARE_PRIVATE(AutocryptRecipient)

    friend class AutocryptStorage;
};
}

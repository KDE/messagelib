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

    [[nodiscard]] QByteArray toJson(QJsonDocument::JsonFormat format) const;
    void fromJson(const QByteArray &json);

    [[nodiscard]] bool hasChanged() const;
    void setChangedFlag(bool changed);

    [[nodiscard]] QByteArray addr() const;
    [[nodiscard]] QByteArray bad_user_agent() const;

    [[nodiscard]] QDateTime last_seen() const;
    [[nodiscard]] QDateTime autocrypt_timestamp() const;
    [[nodiscard]] QDateTime counting_since() const;
    [[nodiscard]] QDateTime gossip_timestamp() const;

    [[nodiscard]] int count_have_ach() const;
    [[nodiscard]] int count_no_ach() const;

    [[nodiscard]] bool prefer_encrypt() const;

    [[nodiscard]] GpgME::Key gpgKey() const;
    [[nodiscard]] QByteArray gpgKeydata() const;

    [[nodiscard]] GpgME::Key gossipKey() const;
    [[nodiscard]] QByteArray gossipKeydata() const;

private:
    std::unique_ptr<AutocryptRecipientPrivate> d_ptr;
    Q_DECLARE_PRIVATE(AutocryptRecipient)

    friend class AutocryptStorage;
};
}

/*
   SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef AUTOCRYPTRECIPIENT_H
#define AUTOCRYPTRECIPIENT_H

#include "messagecore_export.h"

#include <KMime/Headers>

#include <gpgme++/data.h>

#include <QJsonDocument>

namespace MessageCore {

class AutocryptRecipientPrivate;
class AutocryptStorage;
class HeaderMixupNodeHelper;

class MESSAGECORE_EXPORT AutocryptRecipient
{
public:
    typedef QSharedPointer<AutocryptRecipient> Ptr;
    AutocryptRecipient();
    void updateFromMessage(const HeaderMixupNodeHelper &mixup, const KMime::Headers::Base *header);
    void updateFromGossip(const HeaderMixupNodeHelper& mixup, const KMime::Headers::Base* header);

    QByteArray toJson(QJsonDocument::JsonFormat format) const;
    void fromJson(const QByteArray &json);

    bool hasChanged() const;
    void setChangedFlag(bool changed);

    QByteArray addr() const;
    QByteArray bad_user_agent() const;

    QDateTime last_seen() const;
    QDateTime autocrypt_timestamp() const;
    QDateTime counting_since() const;
    QDateTime gossip_timestamp() const;

    int count_have_ach() const;
    int count_no_ach() const;

    bool prefer_encrypt() const;

    GpgME::Key gpgKey() const;
    QByteArray gpgKeydata() const;

    GpgME::Key gossipKey() const;
    QByteArray gossipKeydata() const;

private:
    std::unique_ptr<AutocryptRecipientPrivate> d_ptr;
    Q_DECLARE_PRIVATE(AutocryptRecipient)

    friend class AutocryptStorage;
};
}
#endif

/*
   SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef AUTOCRYPTRECIPIENT_P_H
#define AUTOCRYPTRECIPIENT_P_H

#include <QJsonDocument>

namespace MessageCore
{
class AutocryptRecipientPrivate
{
public:
    AutocryptRecipientPrivate();
    QByteArray toJson(QJsonDocument::JsonFormat format) const;
    void fromJson(const QByteArray &json);

public:
    QByteArray addr;
    QByteArray bad_user_agent;
    QByteArray keydata;
    QByteArray gossip_key;
    QDateTime last_seen;
    QDateTime autocrypt_timestamp;
    QDateTime counting_since;
    QDateTime gossip_timestamp;
    int count_have_ach;
    int count_no_ach;
    bool prefer_encrypt;
    bool changed;
};

}

#endif

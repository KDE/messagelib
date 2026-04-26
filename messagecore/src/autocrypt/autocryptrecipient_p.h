/*
   SPDX-FileCopyrightText: 2020 Sandro Knauß <knauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QJsonDocument>

namespace MessageCore
{
class AutocryptRecipientPrivate
{
public:
    AutocryptRecipientPrivate();
    [[nodiscard]] QByteArray toJson(QJsonDocument::JsonFormat format) const;
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
    int count_have_ach = 0;
    int count_no_ach = 0;
    bool prefer_encrypt = false;
    bool changed = false;
};

}

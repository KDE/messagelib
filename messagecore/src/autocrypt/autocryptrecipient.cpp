/*
   SPDX-FileCopyrightText: 2020 Sandro Kanuß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "autocryptrecipient.h"
using namespace Qt::Literals::StringLiterals;

#include "autocryptrecipient_p.h"
#include "autocryptutils.h"

#include <KCodecs>
#include <QGpgME/DataProvider>
#include <QGpgME/Protocol>
#include <QJsonObject>
#include <gpgme++/data.h>

using namespace MessageCore;

AutocryptRecipientPrivate::AutocryptRecipientPrivate() = default;

QByteArray AutocryptRecipientPrivate::toJson(QJsonDocument::JsonFormat format) const
{
    QJsonObject entry;
    entry.insert(u"addr"_s, QString::fromLatin1(addr));
    entry.insert(u"prefer_encrypt"_s, prefer_encrypt);
    entry.insert(u"keydata"_s, QString::fromLatin1(keydata));
    entry.insert(u"autocrypt_timestamp"_s, autocrypt_timestamp.toString(Qt::ISODate));
    entry.insert(u"count_have_ach"_s, count_have_ach);
    entry.insert(u"count_no_ach"_s, count_no_ach);
    if (last_seen.isValid()) {
        entry.insert(u"last_seen"_s, last_seen.toString(Qt::ISODate));
    }
    if (counting_since.isValid()) {
        entry.insert(u"counting_since"_s, counting_since.toString(Qt::ISODate));
    }
    if (!bad_user_agent.isEmpty()) {
        entry.insert(u"bad_user_agent"_s, QString::fromLatin1(bad_user_agent));
    }
    if (gossip_timestamp.isValid()) {
        entry.insert(u"gossip_timestamp"_s, gossip_timestamp.toString(Qt::ISODate));
        entry.insert(u"gossip_key"_s, QString::fromLatin1(gossip_key));
    }
    QJsonDocument document;
    document.setObject(entry);
    return document.toJson(format);
}

void AutocryptRecipientPrivate::fromJson(const QByteArray &json)
{
    auto document = QJsonDocument::fromJson(json);
    assert(document.isObject());
    const auto &obj = document.object();

    addr = obj.value(u"addr"_s).toString().toLatin1();
    count_have_ach = obj.value(u"count_have_ach"_s).toInt();
    count_no_ach = obj.value(u"count_no_ach"_s).toInt();
    prefer_encrypt = obj.value(u"prefer_encrypt"_s).toBool();
    keydata = obj.value(u"keydata"_s).toString().toLatin1();
    autocrypt_timestamp = QDateTime::fromString(obj.value(u"autocrypt_timestamp"_s).toString(), Qt::ISODate);

    if (obj.contains(u"last_seen"_s)) {
        last_seen = QDateTime::fromString(obj.value(u"last_seen"_s).toString(), Qt::ISODate);
    } else {
        last_seen = QDateTime();
    }

    if (obj.contains(u"counting_since"_s)) {
        counting_since = QDateTime::fromString(obj.value(u"counting_since"_s).toString(), Qt::ISODate);
    } else {
        counting_since = QDateTime();
    }

    if (obj.contains(u"bad_user_agent"_s)) {
        bad_user_agent = obj.value(u"bad_user_agent"_s).toString().toLatin1();
    } else {
        bad_user_agent = "";
    }

    if (obj.contains(u"gossip_timestamp"_s)) {
        gossip_timestamp = QDateTime::fromString(obj.value(u"gossip_timestamp"_s).toString(), Qt::ISODate);
        gossip_key = obj.value(u"gossip_key"_s).toString().toLatin1();
    } else {
        gossip_timestamp = QDateTime();
        gossip_key = "";
    }
    changed = false;
}

AutocryptRecipient::AutocryptRecipient()
    : d_ptr(new AutocryptRecipientPrivate())
{
}

AutocryptRecipient::~AutocryptRecipient() = default;

void AutocryptRecipient::updateFromMessage(const HeaderMixupNodeHelper &mixup, const KMime::Headers::Base *header)
{
    Q_D(AutocryptRecipient);
    QDateTime effectiveDate = mixup.dateHeader();

    if (effectiveDate > QDateTime::currentDateTime()) {
        return;
    }

    if (d->autocrypt_timestamp.isValid() && effectiveDate <= d->autocrypt_timestamp) {
        return;
    }

    d->autocrypt_timestamp = effectiveDate;
    d->changed = true;

    if (!d->counting_since.isValid()) {
        d->counting_since = effectiveDate;
        d->count_have_ach = 0;
        d->count_no_ach = 0;
    }

    if (header) {
        const auto params = paramsFromAutocryptHeader(header);
        if (d->addr.isEmpty()) {
            d->addr = params.value("addr");
        }
        d->prefer_encrypt = params.contains("prefer-encrypt");
        d->keydata = params.value("keydata");
        d->keydata.replace(' ', QByteArray());

        d->last_seen = effectiveDate;
        d->count_have_ach += 1;
    } else {
        d->count_no_ach += 1;
        if (mixup.hasMailHeader("User-Agent")) {
            d->bad_user_agent = mixup.mailHeaderAsBase("User-Agent")->as7BitString(false);
        }
    }
}

void AutocryptRecipient::updateFromGossip(const HeaderMixupNodeHelper &mixup, const KMime::Headers::Base *header)
{
    Q_D(AutocryptRecipient);
    QDateTime effectiveDate = mixup.dateHeader();

    if (effectiveDate > QDateTime::currentDateTime()) {
        return;
    }

    if (d->gossip_timestamp.isValid() && effectiveDate <= d->gossip_timestamp) {
        return;
    }

    const auto params = paramsFromAutocryptHeader(header);

    if (d->addr.isEmpty()) {
        d->addr = params.value("addr");
    } else if (d->addr != params.value("addr")) {
        return;
    }

    d->changed = true;
    d->gossip_timestamp = effectiveDate;
    d->gossip_key = params.value("keydata");
    d->gossip_key.replace(' ', QByteArray());
}

QByteArray AutocryptRecipient::toJson(QJsonDocument::JsonFormat format) const
{
    const Q_D(AutocryptRecipient);
    return d->toJson(format);
}

void AutocryptRecipient::fromJson(const QByteArray &json)
{
    Q_D(AutocryptRecipient);
    return d->fromJson(json);
}

bool AutocryptRecipient::hasChanged() const
{
    const Q_D(AutocryptRecipient);
    return d->changed;
}

void AutocryptRecipient::setChangedFlag(bool changed)
{
    Q_D(AutocryptRecipient);
    d->changed = changed;
}

static GpgME::Key gpgKey(const QByteArray &keydata)
{
    assert(QGpgME::openpgp()); // Make sure, that openpgp backend is loaded
    auto context = GpgME::Context::create(GpgME::OpenPGP);
    QGpgME::QByteArrayDataProvider dp(KCodecs::base64Decode(keydata));
    GpgME::Data data(&dp);
    const auto keys = data.toKeys();
    if (keys.size() == 0) {
        return {};
    }
    return keys[0];
}

GpgME::Key MessageCore::AutocryptRecipient::gpgKey() const
{
    const Q_D(AutocryptRecipient);
    return ::gpgKey(d->keydata);
}

QByteArray MessageCore::AutocryptRecipient::gpgKeydata() const
{
    const Q_D(AutocryptRecipient);
    return KCodecs::base64Decode(d->keydata);
}

GpgME::Key AutocryptRecipient::gossipKey() const
{
    const Q_D(AutocryptRecipient);
    return ::gpgKey(d->gossip_key);
}

QByteArray MessageCore::AutocryptRecipient::gossipKeydata() const
{
    const Q_D(AutocryptRecipient);
    return KCodecs::base64Decode(d->gossip_key);
}

QByteArray AutocryptRecipient::addr() const
{
    const Q_D(AutocryptRecipient);
    return d->addr;
}

QByteArray AutocryptRecipient::bad_user_agent() const
{
    const Q_D(AutocryptRecipient);
    return d->bad_user_agent;
}

QDateTime AutocryptRecipient::last_seen() const
{
    const Q_D(AutocryptRecipient);
    return d->last_seen;
}

QDateTime AutocryptRecipient::autocrypt_timestamp() const
{
    const Q_D(AutocryptRecipient);
    return d->autocrypt_timestamp;
}

QDateTime AutocryptRecipient::counting_since() const
{
    const Q_D(AutocryptRecipient);
    return d->counting_since;
}

QDateTime AutocryptRecipient::gossip_timestamp() const
{
    const Q_D(AutocryptRecipient);
    return d->gossip_timestamp;
}

int AutocryptRecipient::count_have_ach() const
{
    const Q_D(AutocryptRecipient);
    return d->count_have_ach;
}

int AutocryptRecipient::count_no_ach() const
{
    const Q_D(AutocryptRecipient);
    return d->count_no_ach;
}

bool AutocryptRecipient::prefer_encrypt() const
{
    const Q_D(AutocryptRecipient);
    return d->prefer_encrypt;
}

/*
   SPDX-FileCopyrightText: 2020 Sandro Kanuß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "autocryptutils.h"

#include "autocryptstorage.h"

#include "autocrypt_debug.h"



#include <QHash>

using namespace MessageCore;

HeaderMixupNodeHelper::HeaderMixupNodeHelper(MimeTreeParser::NodeHelper *n, KMime::Content *m)
    : message(m)
    , mNodeHelper(n)
{
}

QDateTime HeaderMixupNodeHelper::dateHeader() const
{
    return mNodeHelper->dateHeader(message);
}

bool HeaderMixupNodeHelper::hasMailHeader(const char *header) const
{
    return mNodeHelper->hasMailHeader(header, message);
}

QSharedPointer<KMime::Headers::Generics::AddressList> HeaderMixupNodeHelper::mailHeaderAsAddressList(const char *header) const
{
    return mNodeHelper->mailHeaderAsAddressList(header, message);
}

const KMime::Headers::Base *HeaderMixupNodeHelper::mailHeaderAsBase(const char *header) const
{
    return mNodeHelper->mailHeaderAsBase(header, message);
}

QVector<MimeTreeParser::MessagePart::Ptr> HeaderMixupNodeHelper::messagePartsOfMailHeader(const char *header) const
{
    return mNodeHelper->messagePartsOfMailHeader(header, message);
}

QVector<KMime::Headers::Base *> HeaderMixupNodeHelper::headers(const char *header) const
{
    return mNodeHelper->headers(header, message);
}

QHash<QByteArray, QByteArray> MessageCore::paramsFromAutocryptHeader(const KMime::Headers::Base *const header)
{
    QHash<QByteArray, QByteArray> params;

    const auto &parts = header->as7BitString(false).split(';');
    for (const auto &part : parts) {
        const auto &i = part.split('=');
        params[i[0].trimmed()] = i[1].trimmed();
    }
    return params;
}

void MessageCore::processAutocryptfromMail(const HeaderMixupNodeHelper &mixup)
{
    auto storage = AutocryptStorage::self();

    const auto fromAddr = mixup.mailHeaderAsAddressList("from");
    if (fromAddr->addresses().size() != 1) {
        qInfo(AUTOCRYPT_LOG) << "Don't update Autocrypt storage, because we have multiple From addresses found in "
                             << mixup.mailHeaderAsBase("Message-ID")->as7BitString(false);
        return;
    }

    if (mixup.hasMailHeader("Autocrypt")) {
        KMime::Headers::Base *header = nullptr;
        int valid = 0;
        for (const auto h : mixup.headers("Autocrypt")) {
            const auto params = paramsFromAutocryptHeader(h);
            if (params.value("addr") != fromAddr->addresses().value(0)) {
                continue;
            }
            bool invalid = false;
            for (const auto &key : params.keys()) {
                if (key == "addr") {
                    continue;
                } else if (key == "prefer-encrypt") {
                    continue;
                } else if (key == "keydata") {
                    continue;
                } else if (key[0] == '_') {
                    continue;
                } else {
                    invalid = true;
                    break;
                }
            }
            if (invalid) {
                continue;
            }
            if (!header) {
                header = h;
            }
            valid++;
        }
        if (valid == 1) {
            const auto params = paramsFromAutocryptHeader(header);
            auto recipient = storage->addRecipient(params.value("addr"));
            recipient->updateFromMessage(mixup, header);
            qInfo(AUTOCRYPT_LOG) << "Update Autocrypt information for " << recipient->addr() << " from "
                                 << mixup.mailHeaderAsBase("Message-ID")->as7BitString(false);
        } else {
            qInfo(AUTOCRYPT_LOG) << "Don't update Autocrypt storage, because we have multiple valid Autocrypt headers found in "
                                 << mixup.mailHeaderAsBase("Message-ID")->as7BitString(false);
        }
    } else {
        auto recipient = storage->getRecipient(fromAddr->addresses().value(0));
        if (recipient) {
            recipient->updateFromMessage(mixup, nullptr);
            qInfo(AUTOCRYPT_LOG) << "Update Autocrypt information for " << recipient->addr() << " from "
                                 << mixup.mailHeaderAsBase("Message-ID")->as7BitString(false);
        }
    }

    if (mixup.hasMailHeader("Autocrypt-Gossip")) {
        MimeTreeParser::MessagePart::Ptr messagePart(nullptr);
        for (const auto &mp : mixup.messagePartsOfMailHeader("Autocrypt-Gossip")) {
            if (mp.staticCast<MimeTreeParser::EncryptedMessagePart>()) {
                messagePart = mp;
                break;
            }
        }
        if (messagePart) {
            for (const auto &header : messagePart->headers("Autocrypt-Gossip")) {
                const auto params = paramsFromAutocryptHeader(header);
                auto recipient = storage->addRecipient(params.value("addr"));
                recipient->updateFromGossip(mixup, header);
                qInfo(AUTOCRYPT_LOG) << "Update Autocrypt Gossip information for" << recipient->addr() << " from "
                                     << mixup.mailHeaderAsBase("Message-ID")->as7BitString(false);
            }
        }
    }
    storage->save();
}

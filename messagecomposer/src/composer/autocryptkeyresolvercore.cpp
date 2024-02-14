/*  SPDX-FileCopyrightText: 2023 Sandro Knauß <sknauss@kde.org>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "composer/autocryptkeyresolvercore.h"

#include <Akonadi/ContactSearchJob>
#include <MessageComposer/ContactPreference>
#include <MessageCore/AutocryptStorage>

#include <messagecomposer_debug.h>

using namespace MessageComposer;

class MessageComposer::AutocryptKeyResolverCorePrivate
{
public:
    QStringList autocrypt_keys;
    QStringList gossip_keys;
};

AutocryptKeyResolverCore::AutocryptKeyResolverCore(bool encrypt, bool sign, GpgME::Protocol format)
    : Kleo::KeyResolverCore(encrypt, sign, format)
    , d(new AutocryptKeyResolverCorePrivate)
{
}

AutocryptKeyResolverCore::~AutocryptKeyResolverCore() = default;

Kleo::KeyResolverCore::Result AutocryptKeyResolverCore::resolve()
{
    auto result = Kleo::KeyResolverCore::resolve();

    if ((result.flags & Kleo::KeyResolverCore::AllResolved)) {
        qCDebug(MESSAGECOMPOSER_LOG) << "We found already for all recipient keys and we don't need to add Autocrypt keys.";
        return result;
    }

    if (!(result.flags & Kleo::KeyResolverCore::OpenPGPOnly)) {
        qCWarning(MESSAGECOMPOSER_LOG) << "Autocrypt is only defined for OpenPGP not for SMIME";
        return result;
    }

    Kleo::KeyResolver::Solution *solution = &result.solution;

    if (solution->protocol != GpgME::OpenPGP) {
        solution = &result.alternative;
    }

    Q_ASSERT(solution->protocol == GpgME::OpenPGP);

    bool allResolved = true;
    const auto storage = MessageCore::AutocryptStorage::self();
    for (const auto &recipient : solution->encryptionKeys.keys()) {
        auto &keys = solution->encryptionKeys[recipient];
        if (keys.size() > 0) { // already keys found
            continue;
        }
        if (recipient == normalizedSender()) { // Own key needs to be in normal key store (Autocrypt do not offer private keys)
            allResolved = false;
            continue;
        }

        const auto rec = storage->getRecipient(recipient.toUtf8());
        GpgME::Key autocryptKey;
        if (rec) {
            const auto key = rec->gpgKey();
            if (!key.isBad() && key.canEncrypt()) {
                d->autocrypt_keys.push_back(recipient);
                autocryptKey = std::move(key);
            } else {
                const auto gossipKey = rec->gossipKey();
                if (!gossipKey.isBad() && gossipKey.canEncrypt()) {
                    d->gossip_keys.push_back(recipient);
                    autocryptKey = std::move(gossipKey);
                }
            }
        }
        if (!autocryptKey.isNull()) {
            keys.push_back(autocryptKey);
        } else {
            allResolved = false;
        }
    }
    if (allResolved) {
        result.flags = Kleo::KeyResolverCore::SolutionFlags(result.flags | Kleo::KeyResolverCore::AllResolved);
        if (solution == &result.alternative) {
            const auto _tmp = std::move(result.solution);
            result.solution = std::move(result.alternative);
            result.alternative = std::move(_tmp);
        }
    }

    return result;
}

bool AutocryptKeyResolverCore::isAutocryptKey(const QString &recipient) const
{
    return d->autocrypt_keys.contains(recipient) || isGossipKey(recipient);
}

bool AutocryptKeyResolverCore::isGossipKey(const QString &recipient) const
{
    return d->gossip_keys.contains(recipient);
}

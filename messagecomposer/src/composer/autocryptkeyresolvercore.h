/*  SPDX-FileCopyrightText: 2023 Sandro Knauß <sknauss@kde.org>
    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"

#include <Libkleo/KeyResolverCore>

#include <memory>

namespace MessageComposer
{
class AutocryptKeyResolverCorePrivate;

class MESSAGECOMPOSER_EXPORT AutocryptKeyResolverCore : public Kleo::KeyResolverCore
{
public:
    explicit AutocryptKeyResolverCore(bool encrypt, bool sign, GpgME::Protocol format = GpgME::UnknownProtocol);
    ~AutocryptKeyResolverCore();

    [[nodiscard]] Kleo::KeyResolverCore::Result resolve();

    [[nodiscard]] bool isAutocryptKey(const QString &recipient) const;
    [[nodiscard]] bool isGossipKey(const QString &recipient) const;

private:
    std::unique_ptr<AutocryptKeyResolverCorePrivate> const d;
};
}

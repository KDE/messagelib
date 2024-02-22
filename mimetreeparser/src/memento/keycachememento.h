/*
  SPDX-FileCopyrightText: 2023 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "cryptobodypartmemento.h"
#include <Libkleo/KeyCache>

#include <QString>

namespace MimeTreeParser
{
class KeyCacheMemento : public CryptoBodyPartMemento
{
    Q_OBJECT
public:
    explicit KeyCacheMemento(const std::shared_ptr<Kleo::KeyCache> &keyCache, GpgME::Protocol protocol);
    ~KeyCacheMemento() override;

    [[nodiscard]] bool start() override;
    void exec() override;

    std::shared_ptr<const Kleo::KeyCache> keyCache() const
    {
        return m_keyCache;
    }

private:
    void slotKeyCacheInitialized();
    std::shared_ptr<Kleo::KeyCache> m_keyCache;
    const GpgME::Protocol m_protocol;
};

} // namespace MimeTreeParser

/*
  SPDX-FileCopyrightText: 2023 Daniel Vrátil <dvratil@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "keycachememento.h"

#include <QTimer>

using namespace MimeTreeParser;

KeyCacheMemento::KeyCacheMemento(const std::shared_ptr<Kleo::KeyCache> &keyCache, GpgME::Protocol protocol)
    : m_keyCache(keyCache)
    , m_protocol(protocol)
{
}

KeyCacheMemento::~KeyCacheMemento() = default;

bool KeyCacheMemento::start()
{
    if (m_keyCache->initialized()) {
        // delay completion
        QTimer::singleShot(0, this, &KeyCacheMemento::slotKeyCacheInitialized);
    } else {
        connect(m_keyCache.get(), &Kleo::KeyCache::keyListingDone, this, &KeyCacheMemento::slotKeyCacheInitialized);
        m_keyCache->startKeyListing(m_protocol);
    }

    setRunning(true);
    return true;
}

void KeyCacheMemento::exec()
{
    setRunning(true);
    if (!m_keyCache->initialized()) {
        // This will start a nested eventloop internally that will block until the
        // key cache is initialized and can return a result.
        m_keyCache->findByFingerprint("");
    }
    setRunning(false);
}

void KeyCacheMemento::slotKeyCacheInitialized()
{
    setRunning(false);
    notify();
}

#include "moc_keycachememento.cpp"

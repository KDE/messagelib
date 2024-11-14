// SPDX-FileCopyrightText: 2024 Volker Krause <vkrause@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QByteArrayView>
#include <QHash>

#include <cstring>

namespace MessageList
{
namespace Core
{

/** Compact storage of the result of an MD5 hash computation, for use in the threading code. */
class MD5Hash
{
public:
    inline MD5Hash()
    {
        std::memset(&data, 0, sizeof(MD5Hash));
    }
    inline MD5Hash(QByteArrayView ba)
    {
        if (ba.size() != sizeof(MD5Hash)) {
            std::memset(&data, 0, sizeof(MD5Hash));
            return;
        }

        std::memcpy(&data, ba.constData(), sizeof(MD5Hash));
    }
    inline MD5Hash(const MD5Hash &other)
    {
        std::memcpy(&data, &other.data, sizeof(MD5Hash));
    }

    inline MD5Hash &operator=(const MD5Hash &other)
    {
        std::memcpy(&data, &other.data, sizeof(MD5Hash));
        return *this;
    }

    inline bool operator==(MD5Hash other) const
    {
        return std::memcmp(&data, &other.data, sizeof(MD5Hash)) == 0;
    }

    inline bool isEmpty() const
    {
        return *reinterpret_cast<const quint64 *>(&data) == 0 && *(reinterpret_cast<const quint64 *>(&data) + 1) == 0;
    }

    uint8_t data[16];
};

inline std::size_t qHash(MD5Hash key, std::size_t seed)
{
    return ::qHash(*reinterpret_cast<quint64 *>(&key.data), seed) ^ ::qHash(*(reinterpret_cast<quint64 *>(&key.data) + 1), seed);
}

}
}

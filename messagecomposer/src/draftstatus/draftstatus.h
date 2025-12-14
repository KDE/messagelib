/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <KMime/Message>
#include <Libkleo/Enum>

namespace MessageComposer
{

class MESSAGECOMPOSER_EXPORT DraftEncryptionState
{
public:
    explicit DraftEncryptionState(const std::shared_ptr<KMime::Message> &msg);

    void setState(bool encrypt);
    void removeState();

    [[nodiscard]] bool encryptionState() const;
    [[nodiscard]] bool isDefined() const;

private:
    const std::shared_ptr<KMime::Message> mMsg;
};

class MESSAGECOMPOSER_EXPORT DraftSignatureState
{
public:
    explicit DraftSignatureState(const std::shared_ptr<KMime::Message> &msg);

    void setState(bool sign);
    void removeState();

    [[nodiscard]] bool signState() const;
    [[nodiscard]] bool isDefined() const;

private:
    const std::shared_ptr<KMime::Message> mMsg;
};

class MESSAGECOMPOSER_EXPORT DraftCryptoMessageFormatState
{
public:
    explicit DraftCryptoMessageFormatState(const std::shared_ptr<KMime::Message> &msg);

    void setState(Kleo::CryptoMessageFormat cryptoMessageFormat);
    void removeState();

    [[nodiscard]] Kleo::CryptoMessageFormat cryptoMessageFormatState() const;
    [[nodiscard]] bool isDefined() const;

private:
    const std::shared_ptr<KMime::Message> mMsg;
};

void MESSAGECOMPOSER_EXPORT removeDraftCryptoHeaders(const std::shared_ptr<KMime::Message> &msg);
}

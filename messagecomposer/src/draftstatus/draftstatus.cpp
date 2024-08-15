/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "draftstatus.h"

#include <MessageCore/AutocryptUtils>

using namespace MessageComposer;

void MessageComposer::removeDraftCryptoHeaders(const KMime::Message::Ptr &msg)
{
    DraftEncryptionState(msg).removeState();
    DraftSignatureState(msg).removeState();
    DraftCryptoMessageFormatState(msg).removeState();
}

DraftEncryptionState::DraftEncryptionState(const KMime::Message::Ptr &msg)
    : mMsg(msg)
{
}

void DraftEncryptionState::setState(bool encrypt)
{
    auto hdr = new KMime::Headers::Generic("X-KMail-EncryptActionEnabled");
    hdr->fromUnicodeString(encrypt ? QStringLiteral("true") : QStringLiteral("false"));
    mMsg->setHeader(hdr);
}

void DraftEncryptionState::removeState()
{
    mMsg->removeHeader("X-KMail-EncryptActionEnabled");
}

bool DraftEncryptionState::encryptionState() const
{
    if (isDefined()) {
        auto hdr = mMsg->headerByType("X-KMail-EncryptActionEnabled");
        return hdr->as7BitString(false).contains("true");
    }

    return false;
}

bool DraftEncryptionState::isDefined() const
{
    return mMsg->hasHeader("X-KMail-EncryptActionEnabled");
}

DraftSignatureState::DraftSignatureState(const KMime::Message::Ptr &msg)
    : mMsg(msg)
{
}

void DraftSignatureState::setState(bool sign)
{
    auto hdr = new KMime::Headers::Generic("X-KMail-SignatureActionEnabled");
    hdr->fromUnicodeString(sign ? QStringLiteral("true") : QStringLiteral("false"));
    mMsg->setHeader(hdr);
}

void DraftSignatureState::removeState()
{
    mMsg->removeHeader("X-KMail-SignatureActionEnabled");
}

bool DraftSignatureState::signState() const
{
    if (isDefined()) {
        auto hdr = mMsg->headerByType("X-KMail-SignatureActionEnabled");
        return hdr->as7BitString(false).contains("true");
    }

    return false;
}

bool DraftSignatureState::isDefined() const
{
    return mMsg->hasHeader("X-KMail-SignatureActionEnabled");
}

DraftCryptoMessageFormatState::DraftCryptoMessageFormatState(const KMime::Message::Ptr &msg)
    : mMsg(msg)
{
}

void DraftCryptoMessageFormatState::setState(Kleo::CryptoMessageFormat cryptoMessageFormat)
{
    auto hdr = new KMime::Headers::Generic("X-KMail-CryptoMessageFormat");
    hdr->fromUnicodeString(QString::number(cryptoMessageFormat));
    mMsg->setHeader(hdr);
}

void DraftCryptoMessageFormatState::removeState()
{
    mMsg->removeHeader("X-KMail-CryptoMessageFormat");
}

Kleo::CryptoMessageFormat DraftCryptoMessageFormatState::cryptoMessageFormatState() const
{
    if (isDefined()) {
        auto hdr = mMsg->headerByType("X-KMail-CryptoMessageFormat");
        return static_cast<Kleo::CryptoMessageFormat>(hdr->asUnicodeString().toInt());
    }

    return Kleo::AutoFormat;
}

bool DraftCryptoMessageFormatState::isDefined() const
{
    return mMsg->hasHeader("X-KMail-CryptoMessageFormat");
}

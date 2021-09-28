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
    DraftEncryptionState(const KMime::Message::Ptr &msg);

    void setState(bool encrypt);
    void removeState();

    Q_REQUIRED_RESULT bool encryptionState() const;
    Q_REQUIRED_RESULT bool isDefined() const;

private:
    KMime::Message::Ptr mMsg;
};

class MESSAGECOMPOSER_EXPORT DraftSignatureState
{
public:
    DraftSignatureState(const KMime::Message::Ptr &msg);

    void setState(bool sign);
    void removeState();

    Q_REQUIRED_RESULT bool signState() const;
    Q_REQUIRED_RESULT bool isDefined() const;

private:
    KMime::Message::Ptr mMsg;
};

class MESSAGECOMPOSER_EXPORT DraftCryptoMessageFormatState
{
public:
    DraftCryptoMessageFormatState(const KMime::Message::Ptr &msg);

    void setState(Kleo::CryptoMessageFormat cryptoMessageFormat);
    void removeState();

    Q_REQUIRED_RESULT Kleo::CryptoMessageFormat cryptoMessageFormatState() const;
    Q_REQUIRED_RESULT bool isDefined() const;
private:
    KMime::Message::Ptr mMsg;
};

void MESSAGECOMPOSER_EXPORT removeDraftCryptoHeaders(const KMime::Message::Ptr &msg);

}


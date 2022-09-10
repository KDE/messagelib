/*
  SPDX-FileCopyrightText: 2021 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"

#include <gpgme++/key.h>

#include <QObject>

#include <memory>

class NearExpiryCheckerTest;

namespace MessageComposer
{
class NearExpiryCheckerPrivate;

class MESSAGECOMPOSER_EXPORT NearExpiryChecker : public QObject
{
    Q_OBJECT
public:
    using Ptr = QSharedPointer<NearExpiryChecker>;
    explicit NearExpiryChecker(int encrKeyNearExpiryThresholdDays,
                               int signKeyNearExpiryThresholdDays,
                               int encrRootCertNearExpNearExpiryCheckerPrivateiryThresholdDays,
                               int signRootCertNearExpiryThresholdDays,
                               int encrChainCertNearExpiryThresholdDays,
                               int signChainCertNearExpiryThresholdDays);

    ~NearExpiryChecker();

    int encryptKeyNearExpiryWarningThresholdInDays() const;
    int signingKeyNearExpiryWarningThresholdInDays() const;

    int encryptRootCertNearExpiryWarningThresholdInDays() const;
    int signingRootCertNearExpiryWarningThresholdInDays() const;

    int encryptChainCertNearExpiryWarningThresholdInDays() const;
    int signingChainCertNearExpiryWarningThresholdInDays() const;

    enum ExpiryInformation { OwnKeyExpired, OwnKeyNearExpiry, OtherKeyExpired, OtherKeyNearExpiry };
    Q_ENUM(ExpiryInformation)

    void checkOwnSigningKey(const GpgME::Key &key) const;
    void checkOwnKey(const GpgME::Key &key) const;
    void checkKey(const GpgME::Key &key) const;

Q_SIGNALS:
    void expiryMessage(const GpgME::Key &key, QString msg, MessageComposer::NearExpiryChecker::ExpiryInformation info, bool isNewMessage) const;

private:
    friend class ::NearExpiryCheckerTest;

    std::unique_ptr<NearExpiryCheckerPrivate> const d;

    double calculateSecsTillExpiriy(const GpgME::Subkey &key) const;

    void checkKeyNearExpiry(const GpgME::Key &key,
                            bool isOwnKey,
                            bool isSigningKey,
                            bool ca = false,
                            int recur_limit = 100,
                            const GpgME::Key &orig_key = GpgME::Key::null) const;
};
}
Q_DECLARE_METATYPE(GpgME::Key)

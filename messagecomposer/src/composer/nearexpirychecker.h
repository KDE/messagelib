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
    explicit NearExpiryChecker(int encrOwnKeyNearExpiryThresholdDays,
                               int encrKeyNearExpiryThresholdDays,
                               int encrRootCertNearExpNearExpiryCheckerPrivateiryThresholdDays,
                               int encrChainCertNearExpiryThresholdDays);

    ~NearExpiryChecker();

    Q_REQUIRED_RESULT int encryptOwnKeyNearExpiryWarningThresholdInDays() const;
    Q_REQUIRED_RESULT int encryptKeyNearExpiryWarningThresholdInDays() const;
    Q_REQUIRED_RESULT int encryptRootCertNearExpiryWarningThresholdInDays() const;
    Q_REQUIRED_RESULT int encryptChainCertNearExpiryWarningThresholdInDays() const;

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

    Q_REQUIRED_RESULT MESSAGECOMPOSER_NO_EXPORT double calculateSecsTillExpiriy(const GpgME::Subkey &key) const;

    MESSAGECOMPOSER_NO_EXPORT void checkKeyNearExpiry(const GpgME::Key &key,
                                                      bool isOwnKey,
                                                      bool isSigningKey,
                                                      bool ca = false,
                                                      int recur_limit = 100,
                                                      const GpgME::Key &orig_key = GpgME::Key::null) const;
};
}
Q_DECLARE_METATYPE(GpgME::Key)

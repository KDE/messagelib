/*
  SPDX-FileCopyrightText: 2022 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QByteArray>

#include <set>

namespace MessageComposer {

class NearExpiryCheckerPrivate {
public:
    int encryptKeyNearExpiryWarningThreshold;
    int signingKeyNearExpiryWarningThreshold;
    int encryptRootCertNearExpiryWarningThreshold;
    int signingRootCertNearExpiryWarningThreshold;
    int encryptChainCertNearExpiryWarningThreshold;
    int signingChainCertNearExpiryWarningThreshold;
    
    std::set<QByteArray> alreadyWarnedFingerprints;
    bool testMode = false;
    double difftime = 0;
};

}

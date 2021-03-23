/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <messageviewer/messageviewersettings.h>
namespace MessageViewer
{
/**
 * @brief The DKIMCheckPolicy class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMCheckPolicy
{
public:
    DKIMCheckPolicy();

    Q_REQUIRED_RESULT int rsaSha1Policy() const;
    Q_REQUIRED_RESULT bool verifySignatureWhenOnlyTest() const;
    void setRsaSha1Policy(int rsaSha1Policy);

    void setVerifySignatureWhenOnlyTest(bool verifySignatureWhenOnlyTest);

    Q_REQUIRED_RESULT bool saveDkimResult() const;
    void setSaveDkimResult(bool saveDkimResult);

    Q_REQUIRED_RESULT int saveKey() const;
    void setSaveKey(int saveKey);

    Q_REQUIRED_RESULT bool autogenerateRule() const;
    void setAutogenerateRule(bool autogenerateRule);

    Q_REQUIRED_RESULT bool checkIfEmailShouldBeSigned() const;
    void setCheckIfEmailShouldBeSigned(bool checkIfEmailShouldBeSigned);

    Q_REQUIRED_RESULT bool useDMarc() const;
    void setUseDMarc(bool useDMarc);

    Q_REQUIRED_RESULT bool useDefaultRules() const;
    void setUseDefaultRules(bool useDefaultRules);

    Q_REQUIRED_RESULT bool useAuthenticationResults() const;
    void setUseAuthenticationResults(bool useAuthenticationResults);

    Q_REQUIRED_RESULT bool useRelaxedParsing() const;
    void setUseRelaxedParsing(bool useRelaxedParsing);

    Q_REQUIRED_RESULT bool useOnlyAuthenticationResults() const;
    void setUseOnlyAuthenticationResults(bool useOnlyAuthenticationResults);

    Q_REQUIRED_RESULT bool autogenerateRuleOnlyIfSenderInSDID() const;
    void setAutogenerateRuleOnlyIfSenderInSDID(bool autogenerateRuleOnlyIfSenderInSDID);

    Q_REQUIRED_RESULT int publicRsaTooSmallPolicy() const;
    void setPublicRsaTooSmallPolicy(int publicRsaTooSmallPolicy);

private:
    int mRsaSha1Policy = -1;
    int mSaveKey = -1;
    int mPublicRsaTooSmallPolicy = -1;
    bool mVerifySignatureWhenOnlyTest = false;
    bool mSaveDkimResult = false;
    bool mAutogenerateRule = false;
    bool mCheckIfEmailShouldBeSigned = false;
    bool mUseDMarc = false;
    bool mUseDefaultRules = false;
    bool mUseAuthenticationResults = false;
    bool mUseRelaxedParsing = false;
    bool mUseOnlyAuthenticationResults = false;
    bool mAutogenerateRuleOnlyIfSenderInSDID = false;
};
}


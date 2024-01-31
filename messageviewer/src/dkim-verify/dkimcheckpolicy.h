/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer/messageviewersettings.h"
#include "messageviewer_export.h"
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

    [[nodiscard]] int rsaSha1Policy() const;
    [[nodiscard]] bool verifySignatureWhenOnlyTest() const;
    void setRsaSha1Policy(int rsaSha1Policy);

    void setVerifySignatureWhenOnlyTest(bool verifySignatureWhenOnlyTest);

    [[nodiscard]] bool saveDkimResult() const;
    void setSaveDkimResult(bool saveDkimResult);

    [[nodiscard]] int saveKey() const;
    void setSaveKey(int saveKey);

    [[nodiscard]] bool autogenerateRule() const;
    void setAutogenerateRule(bool autogenerateRule);

    [[nodiscard]] bool checkIfEmailShouldBeSigned() const;
    void setCheckIfEmailShouldBeSigned(bool checkIfEmailShouldBeSigned);

    [[nodiscard]] bool useDMarc() const;
    void setUseDMarc(bool useDMarc);

    [[nodiscard]] bool useDefaultRules() const;
    void setUseDefaultRules(bool useDefaultRules);

    [[nodiscard]] bool useAuthenticationResults() const;
    void setUseAuthenticationResults(bool useAuthenticationResults);

    [[nodiscard]] bool useRelaxedParsing() const;
    void setUseRelaxedParsing(bool useRelaxedParsing);

    [[nodiscard]] bool useOnlyAuthenticationResults() const;
    void setUseOnlyAuthenticationResults(bool useOnlyAuthenticationResults);

    [[nodiscard]] bool autogenerateRuleOnlyIfSenderInSDID() const;
    void setAutogenerateRuleOnlyIfSenderInSDID(bool autogenerateRuleOnlyIfSenderInSDID);

    [[nodiscard]] int publicRsaTooSmallPolicy() const;
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

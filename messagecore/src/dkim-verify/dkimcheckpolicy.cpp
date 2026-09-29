/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimcheckpolicy.h"
#include <utility>

using namespace MessageCore;

DKIMCheckPolicy::DKIMCheckPolicy() = default;

void DKIMCheckPolicy::setKeyChangeApproval(KeyChangeApproval approval)
{
    mKeyChangeApproval = std::move(approval);
}

const DKIMCheckPolicy::KeyChangeApproval &DKIMCheckPolicy::keyChangeApproval() const
{
    return mKeyChangeApproval;
}

int DKIMCheckPolicy::rsaSha1Policy() const
{
    return mRsaSha1Policy;
}

bool DKIMCheckPolicy::verifySignatureWhenOnlyTest() const
{
    return mVerifySignatureWhenOnlyTest;
}

void DKIMCheckPolicy::setRsaSha1Policy(int rsaSha1Policy)
{
    mRsaSha1Policy = rsaSha1Policy;
}

void DKIMCheckPolicy::setVerifySignatureWhenOnlyTest(bool verifySignatureWhenOnlyTest)
{
    mVerifySignatureWhenOnlyTest = verifySignatureWhenOnlyTest;
}

bool DKIMCheckPolicy::saveDkimResult() const
{
    return mSaveDkimResult;
}

void DKIMCheckPolicy::setSaveDkimResult(bool saveDkimResult)
{
    mSaveDkimResult = saveDkimResult;
}

int DKIMCheckPolicy::saveKey() const
{
    return mSaveKey;
}

void DKIMCheckPolicy::setSaveKey(int saveKey)
{
    mSaveKey = saveKey;
}

bool DKIMCheckPolicy::autogenerateRule() const
{
    return mAutogenerateRule;
}

void DKIMCheckPolicy::setAutogenerateRule(bool autogenerateRule)
{
    mAutogenerateRule = autogenerateRule;
}

bool DKIMCheckPolicy::checkIfEmailShouldBeSigned() const
{
    return mCheckIfEmailShouldBeSigned;
}

void DKIMCheckPolicy::setCheckIfEmailShouldBeSigned(bool checkIfEmailShouldBeSigned)
{
    mCheckIfEmailShouldBeSigned = checkIfEmailShouldBeSigned;
}

bool DKIMCheckPolicy::useDMarc() const
{
    return mUseDMarc;
}

void DKIMCheckPolicy::setUseDMarc(bool useDMarc)
{
    mUseDMarc = useDMarc;
}

bool DKIMCheckPolicy::useDefaultRules() const
{
    return mUseDefaultRules;
}

void DKIMCheckPolicy::setUseDefaultRules(bool useDefaultRules)
{
    mUseDefaultRules = useDefaultRules;
}

bool DKIMCheckPolicy::useAuthenticationResults() const
{
    return mUseAuthenticationResults;
}

void DKIMCheckPolicy::setUseAuthenticationResults(bool useAuthenticationResults)
{
    mUseAuthenticationResults = useAuthenticationResults;
}

bool DKIMCheckPolicy::useRelaxedParsing() const
{
    return mUseRelaxedParsing;
}

void DKIMCheckPolicy::setUseRelaxedParsing(bool useRelaxedParsing)
{
    mUseRelaxedParsing = useRelaxedParsing;
}

bool DKIMCheckPolicy::useOnlyAuthenticationResults() const
{
    return mUseOnlyAuthenticationResults;
}

void DKIMCheckPolicy::setUseOnlyAuthenticationResults(bool useOnlyAuthenticationResults)
{
    mUseOnlyAuthenticationResults = useOnlyAuthenticationResults;
}

bool DKIMCheckPolicy::autogenerateRuleOnlyIfSenderInSDID() const
{
    return mAutogenerateRuleOnlyIfSenderInSDID;
}

void DKIMCheckPolicy::setAutogenerateRuleOnlyIfSenderInSDID(bool autogenerateRuleOnlyIfSenderInSDID)
{
    mAutogenerateRuleOnlyIfSenderInSDID = autogenerateRuleOnlyIfSenderInSDID;
}

int DKIMCheckPolicy::publicRsaTooSmallPolicy() const
{
    return mPublicRsaTooSmallPolicy;
}

void DKIMCheckPolicy::setPublicRsaTooSmallPolicy(int publicRsaTooSmallPolicy)
{
    mPublicRsaTooSmallPolicy = publicRsaTooSmallPolicy;
}

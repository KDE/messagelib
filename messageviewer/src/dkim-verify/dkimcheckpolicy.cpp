/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimcheckpolicy.h"

using namespace MessageViewer;

DKIMCheckPolicy::DKIMCheckPolicy()
{
    // HACK make sure we create an instance of MessageViewerSettings, rather than
    // have the below static functions call self() of the base class, which will
    // assert in subsequent use of MessageViewerSettings
    MessageViewer::MessageViewerSettings::self();
    mRsaSha1Policy = MessageViewer::MessageViewerSettings::policyRsaSha1();
    mVerifySignatureWhenOnlyTest = MessageViewer::MessageViewerSettings::verifySignatureWhenOnlyTest();
    mSaveDkimResult = MessageViewer::MessageViewerSettings::saveDkimResult();
    mSaveKey = MessageViewer::MessageViewerSettings::saveKey();
    mAutogenerateRule = MessageViewer::MessageViewerSettings::autogenerateRule();
    mCheckIfEmailShouldBeSigned = MessageViewer::MessageViewerSettings::checkIfEmailShouldBeSigned();
    mUseDMarc = MessageViewer::MessageViewerSettings::useDMarc();
    mUseDefaultRules = MessageViewer::MessageViewerSettings::useDefaultRules();
    mUseAuthenticationResults = MessageViewer::MessageViewerSettings::useAuthenticationResults();
    mUseRelaxedParsing = MessageViewer::MessageViewerSettings::useRelaxedParsingAuthenticationResults();
    mUseOnlyAuthenticationResults = MessageViewer::MessageViewerSettings::useOnlyAuthenticationResults();
    mAutogenerateRuleOnlyIfSenderInSDID = MessageViewer::MessageViewerSettings::autogenerateRuleOnlyIfSenderOnSDID();
    mPublicRsaTooSmallPolicy = MessageViewer::MessageViewerSettings::publicRsaTooSmall();
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

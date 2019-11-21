/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "dkimcheckpolicy.h"

using namespace MessageViewer;

DKIMCheckPolicy::DKIMCheckPolicy()
{
    mRsaSha1Policy = MessageViewer::MessageViewerSettings::policyRsaSha1();
    mVerifySignatureWhenOnlyTest = MessageViewer::MessageViewerSettings::verifySignatureWhenOnlyTest();
    mSaveDkimResult = MessageViewer::MessageViewerSettings::saveDkimResult();
    mSaveKey = MessageViewer::MessageViewerSettings::saveKey();
    mAutogenerateRule = MessageViewer::MessageViewerSettings::autogenerateRule();
    mCheckIfEmailShouldBeSigned = MessageViewer::MessageViewerSettings::checkIfEmailShouldBeSigned();
    mUseDMarc = MessageViewer::MessageViewerSettings::useDMarc();
    mUseDefaultRules = MessageViewer::MessageViewerSettings::useDefaultRules();
    mUseAuthenticationResults = MessageViewer::MessageViewerSettings::useAuthenticationResults();
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

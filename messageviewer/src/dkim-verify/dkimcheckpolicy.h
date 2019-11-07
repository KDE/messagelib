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

#ifndef DKIMCHECKPOLICY_H
#define DKIMCHECKPOLICY_H

#include "messageviewer_export.h"
#include <messageviewer/messageviewersettings.h>
#include <QObject>
namespace MessageViewer {
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

    Q_REQUIRED_RESULT bool saveKey() const;
    void setSaveKey(bool saveKey);

    Q_REQUIRED_RESULT bool autogenerateRule() const;
    void setAutogenerateRule(bool autogenerateRule);

    Q_REQUIRED_RESULT bool checkIfEmailShouldBeSigned() const;
    void setCheckIfEmailShouldBeSigned(bool checkIfEmailShouldBeSigned);

    Q_REQUIRED_RESULT bool useDMarc() const;
    void setUseDMarc(bool useDMarc);

    Q_REQUIRED_RESULT bool useDefaultRules() const;
    void setUseDefaultRules(bool useDefaultRules);

private:
    int mRsaSha1Policy = -1;
    bool mVerifySignatureWhenOnlyTest = false;
    bool mSaveDkimResult = false;
    bool mSaveKey = false;
    bool mAutogenerateRule = false;
    bool mCheckIfEmailShouldBeSigned = false;
    bool mUseDMarc = false;
    bool mUseDefaultRules = false;
};
}

#endif // DKIMCHECKPOLICY_H

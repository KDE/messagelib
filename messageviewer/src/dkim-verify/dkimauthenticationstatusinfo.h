/*
   Copyright (C) 2018-2019 Laurent Montel <montel@kde.org>

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

#ifndef DKIMAUTHENTICATIONSTATUSINFO_H
#define DKIMAUTHENTICATIONSTATUSINFO_H

#include "messageviewer_private_export.h"
#include <QString>

namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT DKIMAuthenticationStatusInfo
{
public:
    DKIMAuthenticationStatusInfo();

    Q_REQUIRED_RESULT QString authservId() const;
    void setAuthservId(const QString &authservId);

    Q_REQUIRED_RESULT bool parseAuthenticationStatus(const QString &key);

    Q_REQUIRED_RESULT QString authVersion() const;
    void setAuthVersion(const QString &authVersion);

    Q_REQUIRED_RESULT QString reasonSpec() const;
    void setReasonSpec(const QString &reasonSpec);

private:
    QString mAuthservId;
    QString mAuthVersion;
    QString mReasonSpec;
};
}

#endif // DKIMAUTHENTICATIONSTATUSINFO_H

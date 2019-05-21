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

#ifndef DKIMINFO_H
#define DKIMINFO_H

#include "messageviewer_private_export.h"
#include <QString>
namespace MessageViewer {
class MESSAGEVIEWER_TESTS_EXPORT DKIMInfo
{
public:
    DKIMInfo();

    void parseDKIM(const QString &header);
    Q_REQUIRED_RESULT QString version() const;
    void setVersion(const QString &version);

    Q_REQUIRED_RESULT QString hashingAlgorithm() const;
    void setHashingAlgorithm(const QString &hashingAlgorithm);

    Q_REQUIRED_RESULT QString domain() const;
    void setDomain(const QString &domain);

    Q_REQUIRED_RESULT QString selector() const;
    void setSelector(const QString &selector);

    Q_REQUIRED_RESULT QString bodyHash() const;
    void setBodyHash(const QString &bodyHash);

    Q_REQUIRED_RESULT bool isValid() const;

private:
    QString mVersion;
    QString mHashingAlgorithm;
    QString mDomain;
    QString mSelector;
    QString mBodyHash;
};
}

#endif // DKIMINFO_H

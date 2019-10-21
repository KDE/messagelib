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

#ifndef DMARCINFO_H
#define DMARCINFO_H

#include "messageviewer_export.h"
#include <QObject>
namespace MessageViewer {
class MESSAGEVIEWER_EXPORT DMARCInfo
{
public:
    DMARCInfo();
    Q_REQUIRED_RESULT bool parseDMARC(const QString &key);
    Q_REQUIRED_RESULT QString version() const;
    void setVersion(const QString &version);

    Q_REQUIRED_RESULT QString adkim() const;
    void setAdkim(const QString &adkim);

    //TODO enum ?
    Q_REQUIRED_RESULT QString policy() const;
    void setPolicy(const QString &policy);

    Q_REQUIRED_RESULT int percentage() const;
    void setPercentage(int percentage);

    Q_REQUIRED_RESULT QString subDomainPolicy() const;
    void setSubDomainPolicy(const QString &subDomainPolicy);

private:
    QString mVersion;
    QString mAdkim;
    QString mPolicy;
    QString mSubDomainPolicy;
    int mPercentage = -1;
};
}

#endif // DMARCINFO_H

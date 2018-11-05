/*
   Copyright (C) 2018 Laurent Montel <montel@kde.org>

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

#include "dkiminfo.h"

using namespace MessageViewer;

DKIMInfo::DKIMInfo()
{

}

void DKIMInfo::parseDKIM()
{
    //TODO
}

QString DKIMInfo::version() const
{
    return mVersion;
}

void DKIMInfo::setVersion(const QString &version)
{
    mVersion = version;
}

QString DKIMInfo::hashingAlgorithm() const
{
    return mHashingAlgorithm;
}

void DKIMInfo::setHashingAlgorithm(const QString &hashingAlgorithm)
{
    mHashingAlgorithm = hashingAlgorithm;
}

QString DKIMInfo::domain() const
{
    return mDomain;
}

void DKIMInfo::setDomain(const QString &domain)
{
    mDomain = domain;
}

QString DKIMInfo::selector() const
{
    return mSelector;
}

void DKIMInfo::setSelector(const QString &selector)
{
    mSelector = selector;
}

QString DKIMInfo::bodyHash() const
{
    return mBodyHash;
}

void DKIMInfo::setBodyHash(const QString &bodyHash)
{
    mBodyHash = bodyHash;
}

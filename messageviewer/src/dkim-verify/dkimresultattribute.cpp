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

#include "dkimresultattribute.h"
#include <QDataStream>

using namespace MessageViewer;
class MessageViewer::DKIMResultAttributePrivate
{
public:
    DKIMResultAttributePrivate()
    {
    }

    //TODO
};

DKIMResultAttribute::DKIMResultAttribute()
    : d(new DKIMResultAttributePrivate)
{
}

DKIMResultAttribute::~DKIMResultAttribute()
{
    delete d;
}

QByteArray DKIMResultAttribute::type() const
{
    static const QByteArray sType("DKIMResultAttribute");
    return sType;
}

DKIMResultAttribute *DKIMResultAttribute::clone() const
{
    DKIMResultAttribute *attr = new DKIMResultAttribute();
    //TODO
    return attr;
}

QByteArray DKIMResultAttribute::serialized() const
{
    QByteArray result;
    QDataStream s(&result, QIODevice::WriteOnly);
    //TODO
    return result;
}

void DKIMResultAttribute::deserialize(const QByteArray &data)
{
    QDataStream s(data);
    //TODO
}

bool DKIMResultAttribute::operator==(const DKIMResultAttribute &other) const
{
    //TODO
    return true;
}

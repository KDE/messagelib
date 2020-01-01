/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

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

    int error = -1;
    int warning = -1;
    int status = -1;
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
    attr->setWarning(warning());
    attr->setStatus(status());
    attr->setError(error());
    return attr;
}

QByteArray DKIMResultAttribute::serialized() const
{
    QByteArray result;
    QDataStream s(&result, QIODevice::WriteOnly);
    s << status();
    s << warning();
    s << error();
    return result;
}

void DKIMResultAttribute::deserialize(const QByteArray &data)
{
    QDataStream s(data);
    int stat = -1;
    s >> stat;
    d->status = stat;
    int warn = -1;
    s >> warn;
    d->warning = warn;
    int err = -1;
    s >> err;
    d->error = err;
}

bool DKIMResultAttribute::operator==(const DKIMResultAttribute &other) const
{
    return d->error == other.error()
           && d->warning == other.warning()
           && d->status == other.status();
}

void DKIMResultAttribute::setError(int err)
{
    d->error = err;
}

int DKIMResultAttribute::error() const
{
    return d->error;
}

void DKIMResultAttribute::setWarning(int err)
{
    d->warning = err;
}

int DKIMResultAttribute::warning() const
{
    return d->warning;
}

void DKIMResultAttribute::setStatus(int err)
{
    d->status = err;
}

int DKIMResultAttribute::status() const
{
    return d->status;
}

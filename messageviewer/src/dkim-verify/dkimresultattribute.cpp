/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimresultattribute.h"
#include <QDataStream>
#include <QIODevice>

using namespace MessageViewer;
class MessageViewer::DKIMResultAttributePrivate
{
public:
    DKIMResultAttributePrivate() = default;

    int error = -1;
    int warning = -1;
    int status = -1;
};

DKIMResultAttribute::DKIMResultAttribute()
    : d(new DKIMResultAttributePrivate)
{
}

DKIMResultAttribute::~DKIMResultAttribute() = default;

QByteArray DKIMResultAttribute::type() const
{
    static const QByteArray sType("DKIMResultAttribute");
    return sType;
}

DKIMResultAttribute *DKIMResultAttribute::clone() const
{
    auto attr = new DKIMResultAttribute();
    attr->setWarning(warning());
    attr->setStatus(status());
    attr->setError(error());
    return attr;
}

QByteArray DKIMResultAttribute::serialized() const
{
    QByteArray result;
    QDataStream s(&result, QIODevice::WriteOnly);
    s.setVersion(QDataStream::Qt_5_15);
    s << status();
    s << warning();
    s << error();
    return result;
}

void DKIMResultAttribute::deserialize(const QByteArray &data)
{
    QDataStream s(data);
    s.setVersion(QDataStream::Qt_5_15);
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
    return d->error == other.error() && d->warning == other.warning() && d->status == other.status();
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

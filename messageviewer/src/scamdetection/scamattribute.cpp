/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "scamattribute.h"
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>

using namespace MessageViewer;

class MessageViewer::ScamAttributePrivate
{
public:
    ScamAttributePrivate() = default;

    bool isAScam = false;
};

ScamAttribute::ScamAttribute()
    : d(new ScamAttributePrivate)
{
}

ScamAttribute::~ScamAttribute() = default;

ScamAttribute *ScamAttribute::clone() const
{
    auto attr = new ScamAttribute();
    attr->setIsAScam(isAScam());
    return attr;
}

QByteArray ScamAttribute::type() const
{
    static const QByteArray sType("ScamAttribute");
    return sType;
}

QByteArray ScamAttribute::serialized() const
{
    QByteArray result;
    QDataStream s(&result, QIODevice::WriteOnly);
    s.setVersion(QDataStream::Qt_5_15);
    s << isAScam();
    return result;
}

void ScamAttribute::deserialize(const QByteArray &data)
{
    QDataStream s(data);
    s.setVersion(QDataStream::Qt_5_15);
    bool value = false;
    s >> value;
    d->isAScam = value;
}

bool ScamAttribute::isAScam() const
{
    return d->isAScam;
}

void ScamAttribute::setIsAScam(bool b)
{
    d->isAScam = b;
}

bool ScamAttribute::operator==(const ScamAttribute &other) const
{
    return d->isAScam == other.isAScam();
}

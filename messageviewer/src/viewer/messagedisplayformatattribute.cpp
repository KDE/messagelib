/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "messagedisplayformatattribute.h"
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>

using namespace MessageViewer;

class MessageViewer::MessageDisplayFormatAttributePrivate
{
public:
    MessageDisplayFormatAttributePrivate() = default;

    Viewer::DisplayFormatMessage messageFormat = Viewer::UseGlobalSetting;
    bool remoteContent = false;
};

MessageDisplayFormatAttribute::MessageDisplayFormatAttribute()
    : d(new MessageDisplayFormatAttributePrivate)
{
}

MessageDisplayFormatAttribute::~MessageDisplayFormatAttribute() = default;

MessageDisplayFormatAttribute *MessageDisplayFormatAttribute::clone() const
{
    auto messageDisplayFormatAttr = new MessageDisplayFormatAttribute();
    messageDisplayFormatAttr->setMessageFormat(messageFormat());
    messageDisplayFormatAttr->setRemoteContent(remoteContent());
    return messageDisplayFormatAttr;
}

QByteArray MessageDisplayFormatAttribute::type() const
{
    static const QByteArray sType("MessageDisplayFormatAttribute");
    return sType;
}

QByteArray MessageDisplayFormatAttribute::serialized() const
{
    QByteArray result;
    QDataStream s(&result, QIODevice::WriteOnly);
    s.setVersion(QDataStream::Qt_5_15);
    s << messageFormat();
    s << remoteContent();

    return result;
}

void MessageDisplayFormatAttribute::setMessageFormat(Viewer::DisplayFormatMessage format)
{
    d->messageFormat = format;
}

void MessageDisplayFormatAttribute::setRemoteContent(bool remote)
{
    d->remoteContent = remote;
}

bool MessageDisplayFormatAttribute::remoteContent() const
{
    return d->remoteContent;
}

bool MessageDisplayFormatAttribute::operator==(const MessageDisplayFormatAttribute &other) const
{
    return (d->messageFormat == other.messageFormat()) && (d->remoteContent == other.remoteContent());
}

Viewer::DisplayFormatMessage MessageDisplayFormatAttribute::messageFormat() const
{
    return d->messageFormat;
}

void MessageDisplayFormatAttribute::deserialize(const QByteArray &data)
{
    QDataStream s(data);
    s.setVersion(QDataStream::Qt_5_15);
    int value = 0;
    s >> value;
    d->messageFormat = static_cast<Viewer::DisplayFormatMessage>(value);
    s >> d->remoteContent;
}

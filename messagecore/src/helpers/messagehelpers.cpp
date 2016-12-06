/*
 * This file is part of KDEPIM.
 * Copyright (c) 2010 Till Adam <adam@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "messagehelpers.h"
#include "messagecore_debug.h"
using namespace MessageCore;
using namespace MessageCore::Util;

KMime::Message::Ptr MessageCore::Util::message(const Akonadi::Item &item)
{
    if (!item.hasPayload<KMime::Message::Ptr>()) {
        qCWarning(MESSAGECORE_LOG) << "Payload is not a MessagePtr!";
        return KMime::Message::Ptr();
    }

    return item.payload<KMime::Message::Ptr>();
}

bool MessageCore::Util::isStandaloneMessage(const Akonadi::Item &item)
{
    // standalone message have a valid payload, but are not, themselves valid items
    return item.hasPayload<KMime::Message::Ptr>() && !item.isValid();
}

void MessageCore::Util::addLinkInformation(const KMime::Message::Ptr &msg, Akonadi::Item::Id id, Akonadi::MessageStatus status)
{
    Q_ASSERT(status.isReplied() || status.isForwarded() || status.isDeleted());

    QString message;
    if (auto hrd = msg->headerByType("X-KMail-Link-Message")) {
        message = hrd->asUnicodeString();
    }
    if (!message.isEmpty()) {
        message += QChar::fromLatin1(',');
    }

    QString type;
    if (auto hrd = msg->headerByType("X-KMail-Link-Type")) {
        type = hrd->asUnicodeString();
    }
    if (!type.isEmpty()) {
        type += QChar::fromLatin1(',');
    }

    message += QString::number(id);
    if (status.isReplied()) {
        type += QLatin1String("reply");
    } else if (status.isForwarded()) {
        type += QLatin1String("forward");
    }

    KMime::Headers::Generic *header = new KMime::Headers::Generic("X-KMail-Link-Message");
    header->fromUnicodeString(message, "utf-8");
    msg->setHeader(header);

    header = new KMime::Headers::Generic("X-KMail-Link-Type");
    header->fromUnicodeString(type, "utf-8");
    msg->setHeader(header);
}

bool MessageCore::Util::getLinkInformation(const KMime::Message::Ptr &msg, QList<Akonadi::Item::Id> &id, QList<Akonadi::MessageStatus> &status)
{
    auto hrdLinkMsg = msg->headerByType("X-KMail-Link-Message");
    auto hrdLinkType = msg->headerByType("X-KMail-Link-Type");
    if (!hrdLinkMsg || !hrdLinkType) {
        return false;
    }

    const QStringList messages = hrdLinkMsg->asUnicodeString().split(QLatin1Char(','), QString::SkipEmptyParts);
    const QStringList types = hrdLinkType->asUnicodeString().split(QLatin1Char(','), QString::SkipEmptyParts);

    if (messages.isEmpty() || types.isEmpty()) {
        return false;
    }

    Q_FOREACH (const QString &idStr, messages) {
        id << idStr.toLongLong();
    }

    Q_FOREACH (const QString &typeStr, types) {
        if (typeStr == QLatin1String("reply")) {
            status << Akonadi::MessageStatus::statusReplied();
        } else if (typeStr == QLatin1String("forward")) {
            status << Akonadi::MessageStatus::statusForwarded();
        }
    }
    return true;
}

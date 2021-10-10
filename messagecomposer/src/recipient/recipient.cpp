/*
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    Based in kmail/recipientseditor.h/cpp
    SPDX-FileCopyrightText: 2004 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#include "recipient.h"

#include <KLocalizedString>

using namespace KPIM;
using namespace MessageComposer;

class MessageComposer::RecipientPrivate
{
public:
    RecipientPrivate(const QString &email, Recipient::Type type)
        : mType(type)
        , mEmail(email)
    {
    }

    Kleo::Action mEncryptionAction = Kleo::Impossible;
    MessageComposer::Recipient::Type mType;
    QString mEmail;
    QString mName;
    GpgME::Key mKey;
};

Recipient::Recipient(const QString &email, Recipient::Type type)
    : d(new MessageComposer::RecipientPrivate(email, type))
{
}

Recipient::~Recipient() = default;

void Recipient::setType(Type type)
{
    d->mType = type;
}

Recipient::Type Recipient::type() const
{
    return d->mType;
}

void Recipient::setEmail(const QString &email)
{
    d->mEmail = email;
}

QString Recipient::email() const
{
    return d->mEmail;
}

void Recipient::setName(const QString &name)
{
    d->mName = name;
}

QString Recipient::name() const
{
    return d->mName;
}

bool Recipient::isEmpty() const
{
    return d->mEmail.isEmpty();
}

void Recipient::clear()
{
    d->mEmail.clear();
    d->mType = Recipient::To;
}

int Recipient::typeToId(Recipient::Type type)
{
    return static_cast<int>(type);
}

Recipient::Type Recipient::idToType(int id)
{
    return static_cast<Type>(id);
}

QString Recipient::typeLabel() const
{
    return typeLabel(d->mType);
}

QString Recipient::typeLabel(Recipient::Type type)
{
    switch (type) {
    case To:
        return i18nc("@label:listbox Recipient of an email message.", "To");
    case Cc:
        return i18nc("@label:listbox Carbon Copy recipient of an email message.", "CC");
    case Bcc:
        return i18nc("@label:listbox Blind carbon copy recipient of an email message.", "BCC");
    case ReplyTo:
        return i18nc("@label:listbox Reply-To recipient of an email message.", "Reply-To");
    case Undefined:
        break;
    }

    return xi18nc("@label:listbox", "<placeholder>Undefined Recipient Type</placeholder>");
}

QStringList Recipient::allTypeLabels()
{
    QStringList types;
    types.append(typeLabel(To));
    types.append(typeLabel(Cc));
    types.append(typeLabel(Bcc));
    types.append(typeLabel(ReplyTo));
    return types;
}

GpgME::Key Recipient::key() const
{
    return d->mKey;
}

void Recipient::setKey(const GpgME::Key &key)
{
    d->mKey = key;
}

Kleo::Action MessageComposer::Recipient::encryptionAction() const
{
    return d->mEncryptionAction;
}

void MessageComposer::Recipient::setEncryptionAction(const Kleo::Action action)
{
    d->mEncryptionAction = action;
}

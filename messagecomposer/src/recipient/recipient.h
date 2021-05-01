/*
    SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
    Based in kmail/recipientseditor.h/cpp
    SPDX-FileCopyrightText: 2004 Cornelius Schumacher <schumacher@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later WITH Qt-Commercial-exception-1.0
*/

#pragma once

#include "messagecomposer_export.h"

#include <Libkdepim/MultiplyingLine>

#include <Libkleo/Enum>
#include <gpgme++/key.h>

#include <QSharedPointer>
#include <QString>
namespace MessageComposer
{
/** Represents a mail recipient. */
class RecipientPrivate;
/**
 * @brief The Recipient class
 */
class MESSAGECOMPOSER_EXPORT Recipient : public KPIM::MultiplyingLineData
{
public:
    using Ptr = QSharedPointer<Recipient>;
    using List = QVector<Recipient::Ptr>;

    enum Type { To, Cc, Bcc, ReplyTo, Undefined };

    Recipient(const QString &email = QString(), Type type = To); // krazy:exclude=explicit
    ~Recipient() override;
    void setType(Type type);
    Q_REQUIRED_RESULT Type type() const;

    void setEmail(const QString &email);
    Q_REQUIRED_RESULT QString email() const;

    void setName(const QString &name);
    Q_REQUIRED_RESULT QString name() const;

    Q_REQUIRED_RESULT bool isEmpty() const override;
    void clear() override;

    Q_REQUIRED_RESULT static int typeToId(Type type);
    Q_REQUIRED_RESULT static Type idToType(int id);

    Q_REQUIRED_RESULT QString typeLabel() const;
    Q_REQUIRED_RESULT static QString typeLabel(Type type);
    Q_REQUIRED_RESULT static QStringList allTypeLabels();

    void setEncryptionAction(const Kleo::Action action);
    Q_REQUIRED_RESULT Kleo::Action encryptionAction() const;

    void setKey(const GpgME::Key &key);
    Q_REQUIRED_RESULT GpgME::Key key() const;

private:
    RecipientPrivate *const d;
};
}


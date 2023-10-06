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
    using List = QList<Recipient::Ptr>;

    enum Type {
        To,
        Cc,
        Bcc,
        ReplyTo,
        Undefined,
    };

    Recipient(const QString &email = QString(), Type type = To); // krazy:exclude=explicit
    ~Recipient() override;
    void setType(Type type);
    [[nodiscard]] Type type() const;

    void setEmail(const QString &email);
    [[nodiscard]] QString email() const;

    void setName(const QString &name);
    [[nodiscard]] QString name() const;

    [[nodiscard]] bool isEmpty() const override;
    void clear() override;

    [[nodiscard]] static int typeToId(Type type);
    [[nodiscard]] static Type idToType(int id);

    [[nodiscard]] QString typeLabel() const;
    [[nodiscard]] static QString typeLabel(Type type);
    [[nodiscard]] static QStringList allTypeLabels();

    void setEncryptionAction(const Kleo::Action action);
    [[nodiscard]] Kleo::Action encryptionAction() const;

    void setKey(const GpgME::Key &key);
    [[nodiscard]] GpgME::Key key() const;

private:
    std::unique_ptr<RecipientPrivate> const d;
};
}

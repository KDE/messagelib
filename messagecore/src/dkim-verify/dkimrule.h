/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messagecore_export.h"
#include <QMetaType>
#include <QStringList>
class QDebug;

namespace MessageCore
{
/*!
 * \class MessageCore::DKIMRule
 * \inmodule MessageCore
 * \inheaderfile MessageCore/DKIMRule
 * \brief The DKIMRule class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGECORE_EXPORT DKIMRule
{
    Q_GADGET
public:
    enum class RuleType : uint8_t {
        Unknown = 0,
        MustBeSigned = 1,
        CanBeSigned = 2,
        IgnoreEmailNotSigned = 3,
    };
    Q_ENUM(RuleType)

    /*!
     */
    DKIMRule();
    /*!
     */
    [[nodiscard]] QString domain() const;
    /*!
     */
    void setDomain(const QString &domain);

    /*!
     */
    [[nodiscard]] QStringList signedDomainIdentifier() const;
    /*!
     */
    void setSignedDomainIdentifier(const QStringList &signedDomainIdentifier);

    /*!
     */
    [[nodiscard]] QString from() const;
    /*!
     */
    void setFrom(const QString &from);

    /*!
     */
    [[nodiscard]] bool enabled() const;
    /*!
     */
    void setEnabled(bool enabled);

    /*!
     */
    [[nodiscard]] bool isValid() const;

    /*!
     */
    [[nodiscard]] RuleType ruleType() const;
    /*!
     */
    void setRuleType(MessageCore::DKIMRule::RuleType ruleType);

    /*!
     */
    [[nodiscard]] QString listId() const;
    /*!
     */
    void setListId(const QString &listId);

    /*!
     */
    [[nodiscard]] bool operator==(const DKIMRule &other) const;
    /*!
     */
    [[nodiscard]] bool operator!=(const DKIMRule &other) const;

    /*!
     */
    [[nodiscard]] int priority() const;
    /*!
     */
    void setPriority(int priority);

private:
    QStringList mSignedDomainIdentifier;
    QString mDomain;
    QString mFrom;
    QString mListId;
    RuleType mRuleType = DKIMRule::RuleType::Unknown;
    int mPriority = 1000;
    bool mEnabled = true;
};
}
Q_DECLARE_TYPEINFO(MessageCore::DKIMRule, Q_RELOCATABLE_TYPE);
MESSAGECORE_EXPORT QDebug operator<<(QDebug d, const MessageCore::DKIMRule &t);

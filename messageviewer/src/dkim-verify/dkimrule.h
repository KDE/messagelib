/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messageviewer_export.h"
#include <QDebug>

namespace MessageViewer
{
/**
 * @brief The DKIMRule class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMRule
{
    Q_GADGET
public:
    enum class RuleType {
        Unknown = 0,
        MustBeSigned = 1,
        CanBeSigned = 2,
        IgnoreEmailNotSigned = 3,
    };
    Q_ENUM(RuleType)

    DKIMRule();
    [[nodiscard]] QString domain() const;
    void setDomain(const QString &domain);

    [[nodiscard]] QStringList signedDomainIdentifier() const;
    void setSignedDomainIdentifier(const QStringList &signedDomainIdentifier);

    [[nodiscard]] QString from() const;
    void setFrom(const QString &from);

    [[nodiscard]] bool enabled() const;
    void setEnabled(bool enabled);

    [[nodiscard]] bool isValid() const;

    [[nodiscard]] RuleType ruleType() const;
    void setRuleType(MessageViewer::DKIMRule::RuleType ruleType);

    [[nodiscard]] QString listId() const;
    void setListId(const QString &listId);

    [[nodiscard]] bool operator==(const DKIMRule &other) const;
    [[nodiscard]] bool operator!=(const DKIMRule &other) const;

    [[nodiscard]] int priority() const;
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
Q_DECLARE_TYPEINFO(MessageViewer::DKIMRule, Q_RELOCATABLE_TYPE);
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::DKIMRule &t);

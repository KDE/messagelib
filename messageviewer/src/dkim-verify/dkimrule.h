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

#ifndef DKIMRULE_H
#define DKIMRULE_H
#include "messageviewer_export.h"
#include <QDebug>
#include <QObject>

namespace MessageViewer {
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
    Q_REQUIRED_RESULT QString domain() const;
    void setDomain(const QString &domain);

    Q_REQUIRED_RESULT QStringList signedDomainIdentifier() const;
    void setSignedDomainIdentifier(const QStringList &signedDomainIdentifier);

    Q_REQUIRED_RESULT QString from() const;
    void setFrom(const QString &from);

    Q_REQUIRED_RESULT bool enabled() const;
    void setEnabled(bool enabled);

    Q_REQUIRED_RESULT bool isValid() const;

    Q_REQUIRED_RESULT RuleType ruleType() const;
    void setRuleType(MessageViewer::DKIMRule::RuleType ruleType);

    Q_REQUIRED_RESULT QString listId() const;
    void setListId(const QString &listId);

    Q_REQUIRED_RESULT bool operator==(const DKIMRule &other) const;
    Q_REQUIRED_RESULT bool operator!=(const DKIMRule &other) const;

    Q_REQUIRED_RESULT int priority() const;
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
Q_DECLARE_METATYPE(MessageViewer::DKIMRule)
Q_DECLARE_TYPEINFO(MessageViewer::DKIMRule, Q_MOVABLE_TYPE);
MESSAGEVIEWER_EXPORT QDebug operator <<(QDebug d, const MessageViewer::DKIMRule &t);

#endif // DKIMRULE_H

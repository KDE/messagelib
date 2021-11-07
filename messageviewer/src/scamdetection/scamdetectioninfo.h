/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/
#pragma once
#include "messageviewer_export.h"
#include <QDebug>
namespace MessageViewer
{
/**
 * @brief The ScamDetectionInfo class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT ScamDetectionInfo
{
public:
    enum ScamDetectionField {
        None = 0,
        DisableAll = 1,
        RedirectUrl = 2,
        HasIp = 4,
    };
    Q_ENUMS(ScamDetectionField)
    Q_DECLARE_FLAGS(ScamDetectionFields, ScamDetectionField)

    ScamDetectionInfo();
    ~ScamDetectionInfo();

    Q_REQUIRED_RESULT const QString &domainOrEmail() const;
    void setDomainOrEmail(const QString &newDomainOrEmail);

    Q_REQUIRED_RESULT bool enabled() const;
    void setEnabled(bool newEnabled);

    Q_REQUIRED_RESULT bool isValid() const;

    Q_REQUIRED_RESULT ScamDetectionInfo::ScamDetectionFields scamChecks() const;

    Q_REQUIRED_RESULT bool operator==(const ScamDetectionInfo &other) const;

private:
    ScamDetectionFields mFields;
    QString mDomainOrEmail;
    bool mEnabled = false;
};
}
Q_DECLARE_TYPEINFO(MessageViewer::ScamDetectionInfo, Q_MOVABLE_TYPE);
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::ScamDetectionInfo &t);

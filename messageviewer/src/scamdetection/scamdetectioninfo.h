/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/
#pragma once
#include "messageviewer_private_export.h"
#include <QDebug>
namespace MessageViewer
{
/**
 * @brief The ScamDetectionInfo class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_TESTS_EXPORT ScamDetectionInfo
{
public:
    ScamDetectionInfo();
    ~ScamDetectionInfo();

    Q_REQUIRED_RESULT const QString &domainOrEmail() const;
    void setDomainOrEmail(const QString &newDomainOrEmail);

    Q_REQUIRED_RESULT bool enabled() const;
    void setEnabled(bool newEnabled);

    Q_REQUIRED_RESULT bool isValid() const;

private:
    QString mDomainOrEmail;
    bool mEnabled = false;
};
}
Q_DECLARE_TYPEINFO(MessageViewer::ScamDetectionInfo, Q_MOVABLE_TYPE);
MESSAGEVIEWER_EXPORT QDebug operator<<(QDebug d, const MessageViewer::ScamDetectionInfo &t);

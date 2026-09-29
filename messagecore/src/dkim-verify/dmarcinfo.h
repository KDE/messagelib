/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"
#include <QMetaType>
class QDebug;
namespace MessageCore
{
/*!
 * \class MessageCore::DMARCInfo
 * \inmodule MessageCore
 * \inheaderfile MessageCore/DMARCInfo
 * \brief The DMARCInfo class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGECORE_EXPORT DMARCInfo
{
public:
    /*!
     */
    DMARCInfo();
    /*!
     */
    [[nodiscard]] bool parseDMARC(const QString &key);
    /*!
     */
    [[nodiscard]] QString version() const;
    /*!
     */
    void setVersion(const QString &version);

    /*!
     */
    [[nodiscard]] QString adkim() const;
    /*!
     */
    void setAdkim(const QString &adkim);

    // TODO enum ?
    /*!
     */
    [[nodiscard]] QString policy() const;
    /*!
     */
    void setPolicy(const QString &policy);

    /*!
     */
    [[nodiscard]] int percentage() const;
    /*!
     */
    void setPercentage(int percentage);

    /*!
     */
    [[nodiscard]] QString subDomainPolicy() const;
    /*!
     */
    void setSubDomainPolicy(const QString &subDomainPolicy);

    /*!
     */
    [[nodiscard]] bool operator==(const DMARCInfo &other) const;

private:
    QString mVersion;
    QString mAdkim;
    QString mPolicy;
    QString mSubDomainPolicy;
    int mPercentage = -1;
};
}
Q_DECLARE_METATYPE(MessageCore::DMARCInfo)
MESSAGECORE_EXPORT QDebug operator<<(QDebug d, const MessageCore::DMARCInfo &t);

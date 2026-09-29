/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"
#include <QObject>
namespace MessageCore
{
/*!
 * \class MessageCore::DKIMManagerAuthenticationServer
 * \inmodule MessageCore
 * \inheaderfile MessageCore/DKIMManagerAuthenticationServer
 * \brief The DKIMManagerAuthenticationServer class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGECORE_EXPORT DKIMManagerAuthenticationServer : public QObject
{
    Q_OBJECT
public:
    /*!
     */
    explicit DKIMManagerAuthenticationServer(QObject *parent = nullptr);
    /*!
     */
    ~DKIMManagerAuthenticationServer() override;
    /*!
     */
    static DKIMManagerAuthenticationServer *self();

    /*!
     */
    [[nodiscard]] QStringList serverList() const;
    /*!
     */
    void setServerList(const QStringList &serverList);

private:
    MESSAGECORE_NO_EXPORT void load();
    MESSAGECORE_NO_EXPORT void save();
    QStringList mServerList;
};
}

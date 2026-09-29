/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkimheaderparser.h"
#include "messagecore_private_export.h"
#include <Akonadi/Item>
#include <QObject>
namespace MessageCore
{
class DKIMAuthenticationStatusInfo;
class MESSAGECORE_EXPORT DKIMCheckAuthenticationStatusJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckAuthenticationStatusJob(QObject *parent = nullptr);
    ~DKIMCheckAuthenticationStatusJob() override;

    void start();

    [[nodiscard]] bool canStart() const;

    void setHeaderParser(const DKIMHeaderParser &headerParser);

    [[nodiscard]] bool useRelaxedParsing() const;
    void setUseRelaxedParsing(bool useRelaxedParsing);

Q_SIGNALS:
    void result(const MessageCore::DKIMAuthenticationStatusInfo &info);

private:
    DKIMHeaderParser mHeaderParser;
    bool mUseRelaxedParsing = false;
};
}

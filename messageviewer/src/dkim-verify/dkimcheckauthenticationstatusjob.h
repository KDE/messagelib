/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "dkimheaderparser.h"
#include "messageviewer_private_export.h"
#include <AkonadiCore/Item>
#include <QObject>
namespace MessageViewer
{
class DKIMAuthenticationStatusInfo;
class MESSAGEVIEWER_TESTS_EXPORT DKIMCheckAuthenticationStatusJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckAuthenticationStatusJob(QObject *parent = nullptr);
    ~DKIMCheckAuthenticationStatusJob() override;

    void start();

    Q_REQUIRED_RESULT bool canStart() const;

    void setHeaderParser(const DKIMHeaderParser &headerParser);

    Q_REQUIRED_RESULT bool useRelaxedParsing() const;
    void setUseRelaxedParsing(bool useRelaxedParsing);

Q_SIGNALS:
    void result(const MessageViewer::DKIMAuthenticationStatusInfo &info);

private:
    DKIMHeaderParser mHeaderParser;
    bool mUseRelaxedParsing = false;
};
}


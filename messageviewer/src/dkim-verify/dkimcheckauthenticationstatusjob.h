/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef DKIMCHECKAUTHENTICATIONSTATUSJOB_H
#define DKIMCHECKAUTHENTICATIONSTATUSJOB_H

#include <QObject>
#include "dkimheaderparser.h"
#include "messageviewer_private_export.h"
#include <AkonadiCore/Item>
namespace MessageViewer {
class DKIMAuthenticationStatusInfo;
class MESSAGEVIEWER_TESTS_EXPORT DKIMCheckAuthenticationStatusJob : public QObject
{
    Q_OBJECT
public:
    explicit DKIMCheckAuthenticationStatusJob(QObject *parent = nullptr);
    ~DKIMCheckAuthenticationStatusJob();

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

#endif // DKIMCHECKAUTHENTICATIONSTATUSJOB_H

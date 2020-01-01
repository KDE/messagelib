/*
   Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

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

/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CIDSCHEMEHANDLER_H
#define CIDSCHEMEHANDLER_H

#include <QObject>
#include <QWebEngineUrlSchemeHandler>
namespace MessageViewer
{
class CidSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT
public:
    explicit CidSchemeHandler(QObject *parent = nullptr);
    ~CidSchemeHandler() override;

    void requestStarted(QWebEngineUrlRequestJob *job) override;
};
}
#endif // CIDSCHEMEHANDLER_H

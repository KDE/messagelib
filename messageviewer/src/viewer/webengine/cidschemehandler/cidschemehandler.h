/*
   SPDX-FileCopyrightText: 2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CIDSCHEMEHANDLER_H
#define CIDSCHEMEHANDLER_H

#include <QWebEngineUrlSchemeHandler>
#include <QObject>
namespace MessageViewer {
class CidSchemeHandler : public QWebEngineUrlSchemeHandler
{
    Q_OBJECT
public:
    explicit CidSchemeHandler(QObject *parent = nullptr);
    ~CidSchemeHandler();

    void requestStarted(QWebEngineUrlRequestJob *job) override;
};
}
#endif // CIDSCHEMEHANDLER_H

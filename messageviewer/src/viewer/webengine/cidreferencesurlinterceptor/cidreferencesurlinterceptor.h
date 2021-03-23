/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>

namespace MessageViewer
{
class CidReferencesUrlInterceptor : public WebEngineViewer::NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    explicit CidReferencesUrlInterceptor(QObject *parent = nullptr);
    ~CidReferencesUrlInterceptor() override;

    Q_REQUIRED_RESULT bool interceptRequest(QWebEngineUrlRequestInfo &info) override;
};
}

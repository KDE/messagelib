/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include <WebEngineViewer/LoadExternalReferencesUrlInterceptor>
#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>
namespace MessageViewer
{
class LoadExternalReferencesUrlInterceptor : public WebEngineViewer::LoadExternalReferencesUrlInterceptor
{
    Q_OBJECT
public:
    explicit LoadExternalReferencesUrlInterceptor(QObject *parent = nullptr);
    ~LoadExternalReferencesUrlInterceptor() override;

protected:
    Q_REQUIRED_RESULT bool urlIsAuthorized(const QUrl &requestedUrl) override;
    Q_REQUIRED_RESULT bool urlIsBlocked(const QUrl &requestedUrl) override;
};
}

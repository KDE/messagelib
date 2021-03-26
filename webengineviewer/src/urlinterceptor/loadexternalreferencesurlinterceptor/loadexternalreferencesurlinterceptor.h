/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "webengineviewer_export.h"
#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>
namespace WebEngineViewer
{
class WEBENGINEVIEWER_EXPORT LoadExternalReferencesUrlInterceptor : public WebEngineViewer::NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    explicit LoadExternalReferencesUrlInterceptor(QObject *parent = nullptr);
    ~LoadExternalReferencesUrlInterceptor() override;

    Q_REQUIRED_RESULT bool interceptRequest(QWebEngineUrlRequestInfo &info) override;
    void setAllowExternalContent(bool b);
    Q_REQUIRED_RESULT bool allowExternalContent() const;

Q_SIGNALS:
    void urlBlocked(const QUrl &url);

protected:
    virtual Q_REQUIRED_RESULT bool urlIsAuthorized(const QUrl &requestedUrl);
    virtual Q_REQUIRED_RESULT bool urlIsBlocked(const QUrl &requestedUrl);

private:
    bool mAllowLoadExternalReference = false;
};
}

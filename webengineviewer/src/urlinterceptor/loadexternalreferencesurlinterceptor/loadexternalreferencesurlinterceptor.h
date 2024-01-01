/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

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

    [[nodiscard]] bool interceptRequest(QWebEngineUrlRequestInfo &info) override;
    void setAllowExternalContent(bool b);
    [[nodiscard]] bool allowExternalContent() const;

Q_SIGNALS:
    void urlBlocked(const QUrl &url);

protected:
    [[nodiscard]] virtual bool urlIsAuthorized(const QUrl &requestedUrl);
    [[nodiscard]] virtual bool urlIsBlocked(const QUrl &requestedUrl);

private:
    bool mAllowLoadExternalReference = false;
};
}

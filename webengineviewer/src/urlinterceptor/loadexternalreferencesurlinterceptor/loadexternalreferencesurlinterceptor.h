/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "webengineviewer_export.h"
#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>
namespace WebEngineViewer
{
/*!
 * \class WebEngineViewer::LoadExternalReferencesUrlInterceptor
 * \inmodule WebEngineViewer
 * \inheaderfile WebEngineViewer/LoadExternalReferencesUrlInterceptor
 *
 * \brief The LoadExternalReferencesUrlInterceptor class
 */
class WEBENGINEVIEWER_EXPORT LoadExternalReferencesUrlInterceptor : public WebEngineViewer::NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    /*! Constructs a LoadExternalReferencesUrlInterceptor with the given parent. */
    explicit LoadExternalReferencesUrlInterceptor(QObject *parent = nullptr);
    /*! Destroys the LoadExternalReferencesUrlInterceptor object. */
    ~LoadExternalReferencesUrlInterceptor() override;

    /*! Intercepts the URL request. */
    [[nodiscard]] bool interceptRequest(QWebEngineUrlRequestInfo &info) override;
    /*! Sets whether external content loading is allowed. */
    void setAllowExternalContent(bool b);
    /*! Returns whether external content loading is allowed. */
    [[nodiscard]] bool allowExternalContent() const;

Q_SIGNALS:
    /*! Emitted when a URL is blocked. */
    void urlBlocked(const QUrl &url);

protected:
    /*! Returns whether the requested URL is authorized. */
    [[nodiscard]] virtual bool urlIsAuthorized(const QUrl &requestedUrl);
    /*! Returns whether the URL is blocked. */
    [[nodiscard]] virtual bool urlIsBlocked(const QUrl &requestedUrl);

private:
    bool mAllowLoadExternalReference = false;
};
}

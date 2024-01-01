/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templatewebenginepage.h"
#include <QWebEngineProfile>
#include <QWebEngineSettings>

using namespace TemplateParser;

TemplateWebEnginePage::TemplateWebEnginePage(QObject *parent)
    : QWebEnginePage(parent)
{
    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
    settings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);
    settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
    settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);
    settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);
    settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled, false);
    settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, false);
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
    // We need to activate it in qt5.15 otherwise we can't load local css file and co
    // settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, false);
    settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, false);
    settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, false);
    settings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, false);
    settings()->setAttribute(QWebEngineSettings::WebGLEnabled, false);
    settings()->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, false);
    settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);
    settings()->setAttribute(QWebEngineSettings::WebGLEnabled, false);

    settings()->setAttribute(QWebEngineSettings::FocusOnNavigationEnabled, false);
    settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
    settings()->setAttribute(QWebEngineSettings::PdfViewerEnabled, false);
    settings()->setAttribute(QWebEngineSettings::NavigateOnDropEnabled, false);
    profile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    profile()->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
}

TemplateWebEnginePage::~TemplateWebEnginePage() = default;

#include "moc_templatewebenginepage.cpp"

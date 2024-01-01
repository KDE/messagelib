/*
   SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "mailwebenginepage.h"
#include <QFontDatabase>
#include <QFontInfo>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

using namespace MessageViewer;

MailWebEnginePage::MailWebEnginePage(QObject *parent)
    : WebEngineViewer::WebEnginePage(parent)
{
    initialize();
}

void MailWebEnginePage::initialize()
{
    const bool enableElement = false;
    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::PluginsEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, enableElement);
    settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, enableElement);
    settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled, enableElement);
    // We need to activate in qt6 otherwise it can't load external url.
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    // We need to activate it in qt5.15 otherwise we can't load local css file and co
    // settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, enableElement);
    settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::WebGLEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, enableElement);
    settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::WebGLEnabled, enableElement);

    settings()->setAttribute(QWebEngineSettings::FocusOnNavigationEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, enableElement);
    settings()->setAttribute(QWebEngineSettings::PdfViewerEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::NavigateOnDropEnabled, enableElement);
    profile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    profile()->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);

    const QFontInfo font(QFontDatabase::systemFont(QFontDatabase::GeneralFont));
    settings()->setFontFamily(QWebEngineSettings::StandardFont, font.family());
    settings()->setFontSize(QWebEngineSettings::DefaultFontSize, font.pixelSize());
    connect(this, &QWebEnginePage::featurePermissionRequested, this, &MailWebEnginePage::slotFeaturePermissionRequested);
}

void MailWebEnginePage::setPrintElementBackground(bool printElementBackground)
{
    settings()->setAttribute(QWebEngineSettings::PrintElementBackgrounds, printElementBackground);
}

void MailWebEnginePage::slotFeaturePermissionRequested(const QUrl &url, QWebEnginePage::Feature feature)
{
    // Denied all permissions.
    setFeaturePermission(url, feature, QWebEnginePage::PermissionDeniedByUser);
}

#include "moc_mailwebenginepage.cpp"

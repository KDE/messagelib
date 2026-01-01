/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

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
    // Default off, needed to load any external urls, we filter them manual
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
    // Default on, needed to load our own icons and cid urls. FIXME: Ideally should be off for additional security
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
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
    connect(this, &QWebEnginePage::permissionRequested, this, &MailWebEnginePage::slotFeaturePermissionRequested);
}

void MailWebEnginePage::setPrintElementBackground(bool printElementBackground)
{
    settings()->setAttribute(QWebEngineSettings::PrintElementBackgrounds, printElementBackground);
}

void MailWebEnginePage::slotFeaturePermissionRequested(QWebEnginePermission feature)
{
    // Denied all permissions.
    feature.deny();
}

#include "moc_mailwebenginepage.cpp"

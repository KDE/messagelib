/*
   Copyright (C) 2016-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "mailwebenginepage.h"
#include <QFontDatabase>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QtWebEngineWidgetsVersion>

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
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, enableElement);
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, enableElement);
    settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::WebGLEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, enableElement);
    settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::WebGLEnabled, enableElement);

    settings()->setAttribute(QWebEngineSettings::FocusOnNavigationEnabled, enableElement);
    settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, enableElement);
#if QTWEBENGINEWIDGETS_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    settings()->setAttribute(QWebEngineSettings::PdfViewerEnabled, enableElement);
#endif
    profile()->setPersistentCookiesPolicy(QWebEngineProfile::NoPersistentCookies);
    profile()->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);

    const QFontInfo font(QFontDatabase().systemFont(QFontDatabase::GeneralFont));
    settings()->setFontFamily(QWebEngineSettings::StandardFont, font.family());
    settings()->setFontSize(QWebEngineSettings::DefaultFontSize, font.pixelSize());
    connect(this, &QWebEnginePage::featurePermissionRequested,
            this, &MailWebEnginePage::slotFeaturePermissionRequested);
}

void MailWebEnginePage::setPrintElementBackground(bool printElementBackground)
{
    settings()->setAttribute(QWebEngineSettings::PrintElementBackgrounds, printElementBackground);
}

void MailWebEnginePage::slotFeaturePermissionRequested(const QUrl &url, QWebEnginePage::Feature feature)
{
    //Denied all permissions.
    setFeaturePermission(url, feature, QWebEnginePage::PermissionDeniedByUser);
}

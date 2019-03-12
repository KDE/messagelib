/*
   Copyright (C) 2016-2019 Laurent Montel <montel@kde.org>

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

using namespace MessageViewer;

MailWebEnginePage::MailWebEnginePage(QObject *parent)
    : WebEngineViewer::WebEnginePage(parent)
{
    initialize();
}

void MailWebEnginePage::initialize()
{
    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, false);
    settings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);
    settings()->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, false);
    settings()->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard, false);
    settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, false);
    settings()->setAttribute(QWebEngineSettings::XSSAuditingEnabled, false);
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, false);
    settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, false);
    settings()->setAttribute(QWebEngineSettings::ErrorPageEnabled, false);
    settings()->setAttribute(QWebEngineSettings::HyperlinkAuditingEnabled, false);
    settings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, false);
    settings()->setAttribute(QWebEngineSettings::WebGLEnabled, false);
    settings()->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, false);
    settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);
    settings()->setAttribute(QWebEngineSettings::WebGLEnabled, false);

    settings()->setAttribute(QWebEngineSettings::FocusOnNavigationEnabled, false);
    settings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, false);
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

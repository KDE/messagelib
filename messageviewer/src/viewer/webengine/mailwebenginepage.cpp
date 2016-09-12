/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

MailWebEnginePage::MailWebEnginePage(QWebEngineProfile *profile, QObject *parent)
    : WebEngineViewer::WebEnginePage(profile, parent)
{
    initialize();
}

MailWebEnginePage::~MailWebEnginePage()
{

}

void MailWebEnginePage::initialize()
{
    profile()->setHttpCacheType(QWebEngineProfile::MemoryHttpCache);
    settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
    settings()->setAttribute(QWebEngineSettings::PluginsEnabled, false);
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    settings()->setAttribute(QWebEngineSettings::AutoLoadIconsForPage, false);
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    settings()->setAttribute(QWebEngineSettings::FocusOnNavigationEnabled, false);
#endif

    const QFontInfo font(QFontDatabase().systemFont(QFontDatabase::GeneralFont));
    settings()->setFontFamily(QWebEngineSettings::StandardFont, font.family());
    settings()->setFontSize(QWebEngineSettings::DefaultFontSize, font.pixelSize());
}

void MailWebEnginePage::setPrintElementBackground(bool printElementBackground)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    settings()->setAttribute(QWebEngineSettings::PrintElementBackgrounds, printElementBackground);
#else
    Q_UNUSED(printElementBackground);
#endif
}

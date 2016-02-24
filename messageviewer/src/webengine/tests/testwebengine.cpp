/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "testwebengine.h"
#include <QHBoxLayout>
#include <QWebEngineView>
#include <MessageViewer/WebEnginePage>
TestWebEngine::TestWebEngine(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *hboxLayout = new QHBoxLayout(this);
    QWebEngineView *pageView = new QWebEngineView(this);
    hboxLayout->addWidget(pageView);
    mEnginePage = new MessageViewer::WebEnginePage(this);
    pageView->setPage(mEnginePage);

    pageView->load(QUrl(QStringLiteral("http://www.kde.org")));
}

TestWebEngine::~TestWebEngine()
{

}

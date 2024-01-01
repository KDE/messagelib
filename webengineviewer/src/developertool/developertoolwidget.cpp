/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "developertoolwidget.h"
#include <QVBoxLayout>
#include <QWebEnginePage>
#include <QWebEngineSettings>
#include <QWebEngineView>
using namespace WebEngineViewer;
DeveloperToolWidget::DeveloperToolWidget(QWidget *parent)
    : QWidget(parent)
    , mWebEngineView(new QWebEngineView(this))
    , mEnginePage(new QWebEnginePage(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));
    mainLayout->setContentsMargins({});

    mWebEngineView->setObjectName(QLatin1StringView("mWebEngineView"));
    mainLayout->addWidget(mWebEngineView);
    mEnginePage->setObjectName(QLatin1StringView("mEnginePage"));
    mEnginePage->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    mWebEngineView->setPage(mEnginePage);
    connect(mEnginePage, &QWebEnginePage::windowCloseRequested, this, &DeveloperToolWidget::closeRequested);
}

DeveloperToolWidget::~DeveloperToolWidget() = default;

QWebEnginePage *DeveloperToolWidget::enginePage() const
{
    return mEnginePage;
}

#include "moc_developertoolwidget.cpp"

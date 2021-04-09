/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

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
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mWebEngineView->setObjectName(QStringLiteral("mWebEngineView"));
    mainLayout->addWidget(mWebEngineView);
    mEnginePage = new QWebEnginePage(this);
    mEnginePage->setObjectName(QStringLiteral("mEnginePage"));
    mEnginePage->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);

    mWebEngineView->setPage(mEnginePage);
}

DeveloperToolWidget::~DeveloperToolWidget()
{
}

QWebEnginePage *DeveloperToolWidget::enginePage() const
{
    return mEnginePage;
}

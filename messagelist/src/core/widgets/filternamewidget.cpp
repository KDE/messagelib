/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filternamewidget.h"
#include <KIconButton>
#include <KLocalizedString>
#include <Libkdepim/LineEditCatchReturnKey>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

using namespace MessageList::Core;
FilterNameWidget::FilterNameWidget(QWidget *parent)
    : QWidget(parent)
    , mName(new QLineEdit(this))
    , mIconButton(new KIconButton(this))
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mName->setObjectName(QStringLiteral("mName"));
    mIconButton->setObjectName(QStringLiteral("mIconButton"));
    mIconButton->setFixedSize(32, 32);
    new KPIM::LineEditCatchReturnKey(mName, this);

    auto label = new QLabel(i18n("Name:"), this);
    label->setObjectName(QStringLiteral("label"));

    mainLayout->addWidget(label);
    mainLayout->addWidget(mName);
    mainLayout->addWidget(mIconButton);
    connect(mName, &QLineEdit::textChanged, this, [this](const QString &str) {
        const QString trimmedStr = str.trimmed();
        Q_EMIT updateOkButton(!trimmedStr.isEmpty() && !mFilterListNames.contains(trimmedStr));
    });
}

FilterNameWidget::~FilterNameWidget()
{
}

QString FilterNameWidget::filterName() const
{
    return mName->text();
}

QString FilterNameWidget::iconName() const
{
    return mIconButton->icon();
}

void FilterNameWidget::setExistingFilterNames(const QStringList &lst)
{
    mFilterListNames = lst;
}

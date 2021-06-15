/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filternamewidget.h"
#include <KLocalizedString>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

using namespace MessageList::Core;
FilterNameWidget::FilterNameWidget(QWidget *parent)
    : QWidget(parent)
    , mName(new QLineEdit(this))
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mName->setObjectName(QStringLiteral("mName"));

    auto label = new QLabel(i18n("Name:"), this);
    label->setObjectName(QStringLiteral("label"));

    mainLayout->addWidget(label);
    mainLayout->addWidget(mName);
    connect(mName, &QLineEdit::textChanged, this, [this](const QString &str) {
        Q_EMIT updateOkButton(!str.trimmed().isEmpty());
    });
}

FilterNameWidget::~FilterNameWidget()
{
}

QString FilterNameWidget::filterName() const
{
    return mName->text();
}

void FilterNameWidget::setExistingFilterNames(const QStringList &lst)
{
    mFilterListNames = lst;
}

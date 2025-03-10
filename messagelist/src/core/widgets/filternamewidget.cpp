/*
  SPDX-FileCopyrightText: 2021-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "filternamewidget.h"
#include <KIconButton>
#include <KLineEditEventHandler>
#include <KLocalizedString>
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
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));
    mainLayout->setContentsMargins({});

    mName->setObjectName(QLatin1StringView("mName"));
    mName->setClearButtonEnabled(true);
    mIconButton->setObjectName(QLatin1StringView("mIconButton"));
    mIconButton->setButtonIconSize(24);
    mIconButton->setFixedSize(32, 32);
    KLineEditEventHandler::catchReturnKey(mName);

    auto label = new QLabel(i18nc("@label:textbox", "Name:"), this);
    label->setObjectName(QLatin1StringView("label"));

    mainLayout->addWidget(label);
    mainLayout->addWidget(mName);
    mainLayout->addWidget(mIconButton);
    connect(mName, &QLineEdit::textChanged, this, [this](const QString &str) {
        const QString trimmedStr = str.trimmed();
        Q_EMIT updateOkButton(!trimmedStr.isEmpty() && !mFilterListNames.contains(trimmedStr));
    });
}

FilterNameWidget::~FilterNameWidget() = default;

void FilterNameWidget::setFilterName(const QString &str)
{
    mName->setText(str);
}

void FilterNameWidget::setIconName(const QString &icon)
{
    mIconButton->setIcon(icon);
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

#include "moc_filternamewidget.cpp"

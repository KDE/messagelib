/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configurefilterswidget.h"
#include "core/filtersavedmanager.h"
#include <KLocalizedString>
#include <QListWidget>
#include <QMenu>
#include <QVBoxLayout>
using namespace MessageList::Core;
ConfigureFiltersWidget::ConfigureFiltersWidget(QWidget *parent)
    : QWidget(parent)
    , mListFiltersWidget(new QListWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});
    mListFiltersWidget->setObjectName(QStringLiteral("mListFiltersWidget"));
    mainLayout->addWidget(mListFiltersWidget);
    mListFiltersWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mListFiltersWidget, &QListWidget::customContextMenuRequested, this, &ConfigureFiltersWidget::slotCustomContextMenuRequested);
    init();
}

ConfigureFiltersWidget::~ConfigureFiltersWidget()
{
}

void ConfigureFiltersWidget::init()
{
    const QVector<FilterSavedManager::FilterInfo> lstFilters = FilterSavedManager::self()->filterInfos();
    for (const auto &filter : lstFilters) {
        mListFiltersWidget->addItem(filter.filterName);
    }
    // TODO load filter
}

void ConfigureFiltersWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
    auto item = mListFiltersWidget->itemAt(pos);
    if (item) {
        QMenu menu(this);
        QString identifier;
        menu.addAction(i18n("Remove"), this, [this, identifier]() {
            removeFilterInfo(identifier);
        });
        menu.exec(QCursor::pos());
    }
}

void ConfigureFiltersWidget::removeFilterInfo(const QString &identifier)
{
    if (!identifier.isEmpty()) {
        FilterSavedManager::self()->removeFilter(identifier);
    }
}

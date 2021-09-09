/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configurefilterswidget.h"
#include "core/filtersavedmanager.h"
#include "filternamedialog.h"
#include <KLocalizedString>
#include <KMessageBox>
#include <QMenu>
#include <QPointer>
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
    connect(mListFiltersWidget, &QListWidget::itemDoubleClicked, this, &ConfigureFiltersWidget::slotConfigureFilter);
    init();
}

ConfigureFiltersWidget::~ConfigureFiltersWidget()
{
}

void ConfigureFiltersWidget::init()
{
    const QVector<FilterSavedManager::FilterInfo> lstFilters = FilterSavedManager::self()->filterInfos();
    for (const auto &filter : lstFilters) {
        auto item = new FilterListWidgetItem(mListFiltersWidget);
        item->setText(filter.filterName);
        item->setIdentifier(filter.identifier);
        item->setIconName(filter.iconName);
        item->setIcon(QIcon::fromTheme(filter.iconName));
        mListFiltersWidget->addItem(item);
    }
}

void ConfigureFiltersWidget::slotConfigureFilter(QListWidgetItem *widgetItem)
{
    if (widgetItem) {
        auto item = static_cast<FilterListWidgetItem *>(widgetItem);
        const QString identifier = item->identifier();
        QPointer<FilterNameDialog> dlg = new FilterNameDialog(this);
        dlg->setFilterName(item->text());
        dlg->setIconName(item->iconName());
        if (dlg->exec()) {
            QString newName = dlg->filterName();
            const QString newIconName = dlg->iconName();
            newName = newName.trimmed();
            if (!newName.isEmpty() && ((newName != item->text()) || (newIconName != item->iconName()))) {
                updateFilterInfo(identifier, newName, newIconName);
                item->setText(newName);
                item->setIconName(newIconName);
                item->setIcon(QIcon::fromTheme(newIconName));
            }
        }
        delete dlg;
    }
}

void ConfigureFiltersWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
    auto item = static_cast<FilterListWidgetItem *>(mListFiltersWidget->itemAt(pos));
    if (item) {
        QMenu menu(this);
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-rename")), i18n("Configure..."), this, [this, item]() {
            slotConfigureFilter(item);
        });
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove"), this, [this, item]() {
            const QString identifier = item->identifier();
            if (KMessageBox::questionYesNo(this, i18n("Do you want to delete this filter?"), i18n("Remove Filter")) == KMessageBox::Yes) {
                removeFilterInfo(identifier);
                delete item;
            }
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

void ConfigureFiltersWidget::updateFilterInfo(const QString &identifier, const QString &newName, const QString &newIconName)
{
    if (!identifier.isEmpty()) {
        FilterSavedManager::self()->updateFilter(identifier, newName, newIconName);
    }
}

FilterListWidgetItem::FilterListWidgetItem(QListWidget *parent)
    : QListWidgetItem(parent)
{
}

FilterListWidgetItem::~FilterListWidgetItem()
{
}

const QString &FilterListWidgetItem::identifier() const
{
    return mIdentifier;
}

void FilterListWidgetItem::setIdentifier(const QString &newIdentifier)
{
    mIdentifier = newIdentifier;
}

const QString &FilterListWidgetItem::iconName() const
{
    return mIconName;
}

void FilterListWidgetItem::setIconName(const QString &newIconName)
{
    mIconName = newIconName;
}

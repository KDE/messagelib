/*
  SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configurefilterswidget.h"
#include "core/filtersavedmanager.h"
#include <KLocalizedString>
#include <KMessageBox>
#include <QInputDialog>
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
        auto item = new FilterListWidgetItem(mListFiltersWidget);
        item->setText(filter.filterName);
        item->setIdentifier(filter.identifier);
        item->setIcon(QIcon::fromTheme(filter.iconName));
        mListFiltersWidget->addItem(item);
    }
}

void ConfigureFiltersWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
    auto item = static_cast<FilterListWidgetItem *>(mListFiltersWidget->itemAt(pos));
    if (item) {
        QMenu menu(this);
        const QString identifier = item->identifier();
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-rename")), i18n("Rename..."), this, [this, identifier, item]() {
            bool ok = false;
            QString newName = QInputDialog::getText(this, i18n("Rename Filter"), i18n("name"), QLineEdit::Normal, item->text(), &ok);
            if (ok) {
                newName = newName.trimmed();
                if (!newName.isEmpty() && (newName != item->text())) {
                    renameFilterInfo(identifier, newName);
                    item->setText(newName);
                }
            }
        });
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove"), this, [this, identifier, item]() {
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

void ConfigureFiltersWidget::renameFilterInfo(const QString &identifier, const QString &newName)
{
    if (!identifier.isEmpty()) {
        FilterSavedManager::self()->renameFilter(identifier, newName);
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

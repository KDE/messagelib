/*
   SPDX-FileCopyrightText: 2018-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeywidget.h"
#include "dkimmanagerkey.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <KTreeWidgetSearchLine>
#include <QApplication>
#include <QClipboard>
#include <QHeaderView>
#include <QMenu>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <kwidgetsaddons_version.h>

using namespace MessageViewer;
DKIMManagerKeyWidget::DKIMManagerKeyWidget(QWidget *parent)
    : QWidget(parent)
    , mTreeWidget(new QTreeWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mainLayout->setContentsMargins({});

    mTreeWidget->setObjectName(QStringLiteral("treewidget"));
    mTreeWidget->setRootIsDecorated(false);
    mTreeWidget->setHeaderLabels({i18n("SDID"), i18n("Selector"), i18n("DKIM Key"), i18n("Inserted"), i18n("Last Used")});
    mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mTreeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    mTreeWidget->setAlternatingRowColors(true);

    auto searchLineEdit = new KTreeWidgetSearchLine(this, mTreeWidget);
    searchLineEdit->setObjectName(QStringLiteral("searchlineedit"));
    searchLineEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(searchLineEdit);

    mainLayout->addWidget(mTreeWidget);
    connect(mTreeWidget, &QTreeWidget::customContextMenuRequested, this, &DKIMManagerKeyWidget::slotCustomContextMenuRequested);
}

DKIMManagerKeyWidget::~DKIMManagerKeyWidget() = default;

QByteArray DKIMManagerKeyWidget::saveHeaders() const
{
    return mTreeWidget->header()->saveState();
}

void DKIMManagerKeyWidget::restoreHeaders(const QByteArray &header)
{
    mTreeWidget->header()->restoreState(header);
}

void DKIMManagerKeyWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = mTreeWidget->itemAt(pos);
    QMenu menu(this);
    if (item) {
        const auto selectedItemsCount{mTreeWidget->selectedItems().count()};
        if (selectedItemsCount == 1) {
            menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), i18n("Copy Key"), this, [item]() {
                QApplication::clipboard()->setText(item->text(2));
            });
            menu.addSeparator();
        }
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")),
                       i18np("Remove Key", "Remove Keys", selectedItemsCount),
                       this,
                       [this, selectedItemsCount]() {
                           const int answer =
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
                               KMessageBox::questionTwoActions(
                                   this,
#else
                                                    KMessageBox::warningYesNo(this,
#endif
                                   i18np("Do you want to delete this key?", "Do you want to delete these keys?", selectedItemsCount),
                                   i18np("Delete Key", "Delete Keys", selectedItemsCount),
                                   KStandardGuiItem::del(),
                                   KStandardGuiItem::cancel());
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
                           if (answer == KMessageBox::ButtonCode::PrimaryAction) {
#else
                           if (answer == KMessageBox::Yes) {
#endif
                               const auto selectedItems = mTreeWidget->selectedItems();
                               for (QTreeWidgetItem *item : selectedItems) {
                                   delete item;
                               }
                           }
                       });
        menu.addSeparator();
    }
    if (mTreeWidget->topLevelItemCount() > 0) {
        menu.addAction(i18n("Delete All"), this, [this]() {
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
            const int answer = KMessageBox::warningTwoActions(this,
#else
            const int answer = KMessageBox::warningYesNo(this,
#endif
                                                              i18n("Do you want to delete all keys?"),
                                                              i18n("Delete Keys"),
                                                              KStandardGuiItem::del(),
                                                              KStandardGuiItem::cancel());
#if KWIDGETSADDONS_VERSION >= QT_VERSION_CHECK(5, 100, 0)
            if (answer == KMessageBox::ButtonCode::PrimaryAction) {
#else
            if (answer == KMessageBox::Yes) {
#endif
                mTreeWidget->clear();
            }
        });
    }
    if (!menu.isEmpty()) {
        menu.exec(QCursor::pos());
    }
}

void DKIMManagerKeyWidget::loadKeys()
{
    const QVector<MessageViewer::KeyInfo> lst = DKIMManagerKey::self()->keys();
    for (const MessageViewer::KeyInfo &key : lst) {
        auto item = new DKIMManagerKeyTreeWidgetItem(mTreeWidget);
        item->setStoredAtDateTime(key.storedAtDateTime);
        item->setLastUsedDateTime(key.lastUsedDateTime);
        item->setText(ManagerKeyTreeWidget::Domain, key.domain);
        item->setText(ManagerKeyTreeWidget::Selector, key.selector);
        item->setText(ManagerKeyTreeWidget::KeyValue, key.keyValue);
        item->setText(ManagerKeyTreeWidget::InsertDate, key.storedAtDateTime.toString());
        item->setText(ManagerKeyTreeWidget::LastUsedDate, key.lastUsedDateTime.toString());
        item->setToolTip(ManagerKeyTreeWidget::KeyValue, key.keyValue);
    }
    mTreeWidget->setSortingEnabled(true);
    mTreeWidget->header()->setSortIndicatorShown(true);
    mTreeWidget->header()->setSectionsClickable(true);
    mTreeWidget->sortByColumn(ManagerKeyTreeWidget::Domain, Qt::AscendingOrder);
}

void DKIMManagerKeyWidget::saveKeys()
{
    QVector<MessageViewer::KeyInfo> lst;
    lst.reserve(mTreeWidget->topLevelItemCount());
    for (int i = 0, total = mTreeWidget->topLevelItemCount(); i < total; ++i) {
        QTreeWidgetItem *item = mTreeWidget->topLevelItem(i);
        const MessageViewer::KeyInfo info{item->text(ManagerKeyTreeWidget::KeyValue),
                                          item->text(ManagerKeyTreeWidget::Selector),
                                          item->text(ManagerKeyTreeWidget::Domain),
                                          QDateTime::fromString(item->text(ManagerKeyTreeWidget::InsertDate)),
                                          QDateTime::fromString(item->text(ManagerKeyTreeWidget::LastUsedDate))};
        lst.append(info);
    }
    DKIMManagerKey::self()->saveKeys(lst);
}

void DKIMManagerKeyWidget::resetKeys()
{
    mTreeWidget->clear();
    loadKeys();
}

DKIMManagerKeyTreeWidgetItem::DKIMManagerKeyTreeWidgetItem(QTreeWidget *parent)
    : QTreeWidgetItem(parent)
{
}

DKIMManagerKeyTreeWidgetItem::~DKIMManagerKeyTreeWidgetItem() = default;

bool DKIMManagerKeyTreeWidgetItem::operator<(const QTreeWidgetItem &other) const
{
    const int column = treeWidget()->sortColumn();
    if (column == DKIMManagerKeyWidget::ManagerKeyTreeWidget::InsertDate) {
        return storedAtDateTime() < static_cast<const DKIMManagerKeyTreeWidgetItem *>(&other)->storedAtDateTime();
    }
    if (column == DKIMManagerKeyWidget::ManagerKeyTreeWidget::LastUsedDate) {
        return lastUsedDateTime() < static_cast<const DKIMManagerKeyTreeWidgetItem *>(&other)->lastUsedDateTime();
    }
    return QTreeWidgetItem::operator<(other);
}

const QDateTime &DKIMManagerKeyTreeWidgetItem::storedAtDateTime() const
{
    return mStoredAtDateTime;
}

void DKIMManagerKeyTreeWidgetItem::setStoredAtDateTime(const QDateTime &newStoredAtDateTime)
{
    mStoredAtDateTime = newStoredAtDateTime;
}

const QDateTime &DKIMManagerKeyTreeWidgetItem::lastUsedDateTime() const
{
    return mLastUsedDateTime;
}

void DKIMManagerKeyTreeWidgetItem::setLastUsedDateTime(const QDateTime &newLastUsedDateTime)
{
    mLastUsedDateTime = newLastUsedDateTime;
}

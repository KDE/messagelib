/*
   SPDX-FileCopyrightText: 2020-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentconfigurewidget.h"
#include "remotecontentdialog.h"
#include "remotecontentmanager.h"
#include "remotecontentstatustypecombobox.h"
#include <KLineEditEventHandler>
#include <KLocalizedString>
#include <KMessageBox>
#include <KTreeWidgetSearchLine>
#include <QHeaderView>
#include <QMenu>
#include <QPointer>
#include <QTreeWidget>
#include <QVBoxLayout>

using namespace MessageViewer;

RemoteContentWidgetItem::RemoteContentWidgetItem(QTreeWidget *parent)
    : QTreeWidgetItem(parent)
    , mStatusTypeCombobox(new RemoteContentStatusTypeComboBox)
{
    treeWidget()->setItemWidget(this, ColumnType::RuleType, mStatusTypeCombobox);
}

RemoteContentWidgetItem::~RemoteContentWidgetItem() = default;

void RemoteContentWidgetItem::setStatus(MessageViewer::RemoteContentInfo::RemoteContentInfoStatus type)
{
    mStatusTypeCombobox->setStatus(type);
}

RemoteContentInfo::RemoteContentInfoStatus RemoteContentWidgetItem::status() const
{
    return mStatusTypeCombobox->status();
}

RemoteContentConfigureWidget::RemoteContentConfigureWidget(QWidget *parent)
    : QWidget(parent)
    , mTreeWidget(new QTreeWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainLayout"));
    mainLayout->setContentsMargins({});

    mTreeWidget->setObjectName(QLatin1StringView("treewidget"));
    mTreeWidget->setRootIsDecorated(false);
    mTreeWidget->setHeaderLabels({i18n("Domain"), i18n("Status")});
    mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mTreeWidget->setAlternatingRowColors(true);
    mTreeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    auto searchLineEdit = new KTreeWidgetSearchLine(this, mTreeWidget);
    searchLineEdit->setObjectName(QLatin1StringView("searchlineedit"));
    searchLineEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(searchLineEdit);
    KLineEditEventHandler::catchReturnKey(searchLineEdit);

    mainLayout->addWidget(mTreeWidget);
    connect(mTreeWidget, &QTreeWidget::customContextMenuRequested, this, &RemoteContentConfigureWidget::slotCustomContextMenuRequested);
    connect(mTreeWidget, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item) {
        if (item) {
            auto rulesItem = static_cast<RemoteContentWidgetItem *>(item);
            modifyRemoteContent(rulesItem);
        }
    });
    readSettings();
}

RemoteContentConfigureWidget::~RemoteContentConfigureWidget() = default;

void RemoteContentConfigureWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = mTreeWidget->itemAt(pos);
    QMenu menu(this);
    menu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add..."), this, &RemoteContentConfigureWidget::slotAdd);
    if (item) {
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("document-edit")), i18n("Modify..."), this, [this, item]() {
            modifyRemoteContent(static_cast<RemoteContentWidgetItem *>(item));
        });
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove Rule"), this, [this, item]() {
            const int answer = KMessageBox::warningTwoActions(this,
                                                              i18n("Do you want to delete this rule '%1'?", item->text(0)),
                                                              i18nc("@title:window", "Delete Rule"),
                                                              KStandardGuiItem::del(),
                                                              KStandardGuiItem::cancel());
            if (answer == KMessageBox::ButtonCode::PrimaryAction) {
                delete item;
            }
        });
    }
    menu.exec(QCursor::pos());
}

void RemoteContentConfigureWidget::modifyRemoteContent(RemoteContentWidgetItem *rulesItem)
{
    if (!rulesItem) {
        return;
    }
    QPointer<RemoteContentDialog> dlg = new RemoteContentDialog(this);
    dlg->setWindowTitle(i18nc("@title:window", "Edit Remote Content"));
    RemoteContentInfo info;
    info.setUrl(rulesItem->text(0));
    info.setStatus(rulesItem->status());
    dlg->setInfo(info);
    if (dlg->exec()) {
        info = dlg->info();
        RemoteContentManager::self()->addRemoteContent(info);
        fillContentInfo(info, rulesItem);
    }
    delete dlg;
}

void RemoteContentConfigureWidget::slotAdd()
{
    QPointer<RemoteContentDialog> dlg = new RemoteContentDialog(this);
    dlg->setWindowTitle(i18nc("@title:window", "Add Remote Content"));
    if (dlg->exec()) {
        const auto info = dlg->info();
        const int count = mTreeWidget->topLevelItemCount();
        bool isUnique = true;
        for (int i = 0; i < count; ++i) {
            const auto item = mTreeWidget->topLevelItem(i);
            if (item->text(0) == info.url()) {
                isUnique = false;
                KMessageBox::error(this, i18n("An entry already defines this url. Please modify it."), i18nc("@title:window", "Add new Url"));
                break;
            }
        }
        if (isUnique) {
            RemoteContentManager::self()->addRemoteContent(info);
            insertRemoteContentInfo(info);
        }
    }
    delete dlg;
}

void RemoteContentConfigureWidget::saveSettings()
{
    QList<RemoteContentInfo> lst;
    const int count = mTreeWidget->topLevelItemCount();
    lst.reserve(count);
    for (int i = 0; i < count; ++i) {
        const auto item = static_cast<RemoteContentWidgetItem *>(mTreeWidget->topLevelItem(i));
        RemoteContentInfo info;
        info.setUrl(item->text(0));
        info.setStatus(item->status());
        lst.append(std::move(info));
    }
    RemoteContentManager::self()->setRemoveContentInfo(lst);
}

void RemoteContentConfigureWidget::readSettings()
{
    const QList<RemoteContentInfo> remoteContentInfos = RemoteContentManager::self()->removeContentInfo();
    for (const RemoteContentInfo &info : remoteContentInfos) {
        insertRemoteContentInfo(info);
    }
    mTreeWidget->setSortingEnabled(true);
    mTreeWidget->header()->setSortIndicatorShown(true);
    mTreeWidget->header()->setSectionsClickable(true);
    mTreeWidget->sortByColumn(0, Qt::AscendingOrder);
}

void RemoteContentConfigureWidget::insertRemoteContentInfo(const RemoteContentInfo &info)
{
    auto item = new RemoteContentWidgetItem(mTreeWidget);
    fillContentInfo(info, item);
}

void RemoteContentConfigureWidget::fillContentInfo(const RemoteContentInfo &info, RemoteContentWidgetItem *item)
{
    item->setText(0, info.url());
    item->setToolTip(0, info.url());
    item->setStatus(info.status());
}

#include "moc_remotecontentconfigurewidget.cpp"

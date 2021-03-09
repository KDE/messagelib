/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "remotecontentconfigurewidget.h"
#include "remotecontentdialog.h"
#include "remotecontentinfo.h"
#include "remotecontentmanager.h"
#include <KLocalizedString>
#include <KMessageBox>
#include <KTreeWidgetSearchLine>
#include <QListWidget>
#include <QMenu>
#include <QPointer>
#include <QTreeWidget>
#include <QVBoxLayout>

using namespace MessageViewer;
RemoteContentConfigureWidget::RemoteContentConfigureWidget(QWidget *parent)
    : QWidget(parent)
    , mTreeWidget(new QTreeWidget(this))
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mTreeWidget->setObjectName(QStringLiteral("treewidget"));
    mTreeWidget->setRootIsDecorated(false);
    mTreeWidget->setHeaderLabels({i18n("Domain"), i18n("Status")});
    mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mTreeWidget->setAlternatingRowColors(true);

    auto searchLineEdit = new KTreeWidgetSearchLine(this, mTreeWidget);
    searchLineEdit->setObjectName(QStringLiteral("searchlineedit"));
    searchLineEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(searchLineEdit);

    mainLayout->addWidget(mTreeWidget);
    connect(mTreeWidget, &QTreeWidget::customContextMenuRequested, this, &RemoteContentConfigureWidget::slotCustomContextMenuRequested);
    //    connect(mTreeWidget, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem *item) {
    //        if (item) {
    //            auto *rulesItem = dynamic_cast<DKIMManageRulesWidgetItem *>(item);
    //            modifyRule(rulesItem);
    //        }
    //    });
}

RemoteContentConfigureWidget::~RemoteContentConfigureWidget()
{
}

void RemoteContentConfigureWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = mTreeWidget->itemAt(pos);
    QMenu menu(this);
    menu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add..."), this, &RemoteContentConfigureWidget::slotAdd);
    if (item) {
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("document-edit")), i18n("Modify..."), this, [this, item]() {
            // modifyRemoteContent(item);
        });
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove Rule"), this, [this, item]() {
            if (KMessageBox::Yes == KMessageBox::warningYesNo(this, i18n("Do you want to delete this rule '%1'?", item->text(0)), i18n("Delete Rule"))) {
                delete item;
            }
        });
    }
    menu.exec(QCursor::pos());
}

void RemoteContentConfigureWidget::modifyRemoteContent(QListWidgetItem *rulesItem)
{
    //    if (!rulesItem) {
    //        return;
    //    }
    //    QPointer<RemoteContentDialog> dlg = new RemoteContentDialog(this);
    //    RemoteContentInfo info;
    //    info.setUrl(rulesItem->text());
    //    info.setStatus(rulesItem->checkState() == Qt::Checked ? RemoteContentInfo::RemoteContentInfoStatus::Authorized
    //                                                          : RemoteContentInfo::RemoteContentInfoStatus::Blocked);
    //    dlg->setInfo(info);
    //    if (dlg->exec()) {
    //        info = dlg->info();
    //        RemoteContentManager::self()->addRemoteContent(info);
    //        rulesItem->setText(info.url());
    //        rulesItem->setCheckState((info.status() == RemoteContentInfo::RemoteContentInfoStatus::Authorized) ? Qt::Checked : Qt::Unchecked);
    //    }
    //    delete dlg;
}

void RemoteContentConfigureWidget::slotAdd()
{
    //    QPointer<RemoteContentDialog> dlg = new RemoteContentDialog(this);
    //    if (dlg->exec()) {
    //        const auto info = dlg->info();
    //        const int count = mListWidget->count();
    //        bool isUnique = true;
    //        for (int i = 0; i < count; ++i) {
    //            const auto item = mListWidget->item(i);
    //            if (item->text() == info.url()) {
    //                isUnique = false;
    //                KMessageBox::error(this, i18n("An entry already defines this url. Please modify it."), i18n("Add new Url"));
    //                break;
    //            }
    //        }
    //        if (isUnique) {
    //            RemoteContentManager::self()->addRemoteContent(info);
    //            insertRemoteContentInfo(info);
    //        }
    //    }
    //    delete dlg;
}

void RemoteContentConfigureWidget::saveSettings()
{
    //    QVector<RemoteContentInfo> lst;
    //    const int count = mListWidget->count();
    //    lst.reserve(count);
    //    for (int i = 0; i < count; ++i) {
    //        const auto item = mListWidget->item(i);
    //        RemoteContentInfo info;
    //        info.setUrl(item->text());
    //        info.setStatus((item->checkState() == Qt::Checked) ? RemoteContentInfo::RemoteContentInfoStatus::Authorized
    //                                                           : RemoteContentInfo::RemoteContentInfoStatus::Blocked);
    //        lst.append(std::move(info));
    //    }
    //    RemoteContentManager::self()->setRemoveContentInfo(lst);
}

void RemoteContentConfigureWidget::readSettings()
{
    const QVector<RemoteContentInfo> remoteContentInfos = RemoteContentManager::self()->removeContentInfo();
    for (const RemoteContentInfo &info : remoteContentInfos) {
        insertRemoteContentInfo(info);
    }
}

void RemoteContentConfigureWidget::insertRemoteContentInfo(const RemoteContentInfo &info)
{
    //    auto item = new QListWidgetItem(mListWidget);
    //    item->setText(info.url());
    //    if (info.status() == RemoteContentInfo::RemoteContentInfoStatus::Authorized) {
    //        item->setCheckState(Qt::Checked);
    //    } else {
    //        item->setCheckState(Qt::Unchecked);
    //    }
}

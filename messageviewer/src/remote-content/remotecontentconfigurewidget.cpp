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
#include <QListWidget>
#include <QMenu>
#include <QPointer>
#include <QVBoxLayout>

using namespace MessageViewer;
RemoteContentConfigureWidget::RemoteContentConfigureWidget(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));
    mainLayout->setContentsMargins({});

    mListWidget = new QListWidget(this);
    mListWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mListWidget->setObjectName(QStringLiteral("mListWidget"));
    mainLayout->addWidget(mListWidget);
    readSettings();
    connect(mListWidget, &QListWidget::customContextMenuRequested, this, &RemoteContentConfigureWidget::slotCustomContextMenuRequested);
    connect(mListWidget, &QListWidget::itemDoubleClicked, this, &RemoteContentConfigureWidget::modifyRemoteContent);
}

RemoteContentConfigureWidget::~RemoteContentConfigureWidget()
{
}

void RemoteContentConfigureWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem *item = mListWidget->itemAt(pos);
    QMenu menu(this);
    menu.addAction(QIcon::fromTheme(QStringLiteral("list-add")), i18n("Add..."), this, &RemoteContentConfigureWidget::slotAdd);
    if (item) {
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("document-edit")), i18n("Modify..."), this, [this, item]() {
            modifyRemoteContent(item);
        });
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove Rule"), this, [this, item]() {
            if (KMessageBox::Yes == KMessageBox::warningYesNo(this, i18n("Do you want to delete this rule?"), i18n("Delete Rule"))) {
                delete item;
            }
        });
    }
    menu.exec(QCursor::pos());
}

void RemoteContentConfigureWidget::modifyRemoteContent(QListWidgetItem *rulesItem)
{
    if (!rulesItem) {
        return;
    }
    QPointer<RemoteContentDialog> dlg = new RemoteContentDialog(this);
    RemoteContentInfo info;
    info.setUrl(rulesItem->text());
    info.setStatus(rulesItem->checkState() == Qt::Checked ? RemoteContentInfo::RemoteContentInfoStatus::Authorized
                                                          : RemoteContentInfo::RemoteContentInfoStatus::Blocked);
    dlg->setInfo(info);
    if (dlg->exec()) {
        info = dlg->info();
        RemoteContentManager::self()->addRemoteContent(info);
        rulesItem->setText(info.url());
        rulesItem->setCheckState((info.status() == RemoteContentInfo::RemoteContentInfoStatus::Authorized) ? Qt::Checked : Qt::Unchecked);
    }
    delete dlg;
}

void RemoteContentConfigureWidget::slotAdd()
{
    QPointer<RemoteContentDialog> dlg = new RemoteContentDialog(this);
    if (dlg->exec()) {
        const auto info = dlg->info();
        if (RemoteContentManager::self()->isUnique(info)) {
            RemoteContentManager::self()->addRemoteContent(info);
            insertRemoteContentInfo(info);
        } else {
            KMessageBox::error(this, i18n("An entry already defines this url. Please modify it."), i18n("Add new Url"));
        }
    }
    delete dlg;
}

void RemoteContentConfigureWidget::saveSettings()
{
    QVector<RemoteContentInfo> lst;
    const int count = mListWidget->count();
    lst.reserve(count);
    for (int i = 0; i < count; ++i) {
        const auto item = mListWidget->item(i);
        RemoteContentInfo info;
        info.setUrl(item->text());
        info.setStatus((item->checkState() == Qt::Checked) ? RemoteContentInfo::RemoteContentInfoStatus::Authorized
                                                           : RemoteContentInfo::RemoteContentInfoStatus::Blocked);
        lst.append(std::move(info));
    }
    RemoteContentManager::self()->setRemoveContentInfo(lst);
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
    auto item = new QListWidgetItem(mListWidget);
    item->setText(info.url());
    if (info.status() == RemoteContentInfo::RemoteContentInfoStatus::Authorized) {
        item->setCheckState(Qt::Checked);
    } else {
        item->setCheckState(Qt::Unchecked);
    }
}

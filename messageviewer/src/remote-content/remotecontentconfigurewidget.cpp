/*
   SPDX-FileCopyrightText: 2020-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "remotecontentconfigurewidget.h"
#include "remotecontentinfo.h"
#include "remotecontentmanager.h"
#include <KLocalizedString>
#include <QListWidget>
#include <QMenu>
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
}

RemoteContentConfigureWidget::~RemoteContentConfigureWidget()
{

}

void RemoteContentConfigureWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem *item = mListWidget->itemAt(pos);
    QMenu menu(this);
    menu.exec(QCursor::pos());
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
        info.setStatus((item->checkState() == Qt::Checked) ? RemoteContentInfo::RemoteContentInfoStatus::Authorized : RemoteContentInfo::RemoteContentInfoStatus::Blocked);
        lst.append(std::move(info));
    }
    RemoteContentManager::self()->setRemoveContentInfo(lst);
}

void RemoteContentConfigureWidget::readSettings()
{
    const QVector<RemoteContentInfo> remoteContentInfos = RemoteContentManager::self()->removeContentInfo();
    for (const RemoteContentInfo &info : remoteContentInfos) {
        auto item = new QListWidgetItem(mListWidget);
        item->setText(info.url());
        if (info.status() == RemoteContentInfo::RemoteContentInfoStatus::Authorized) {
            item->setCheckState(Qt::Checked);
        }
    }
}

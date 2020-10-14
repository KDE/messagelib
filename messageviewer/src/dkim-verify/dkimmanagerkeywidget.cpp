/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeywidget.h"
#include "dkimmanagerkey.h"

#include <QTreeWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <KLocalizedString>
#include <QMenu>
#include <QClipboard>
#include <QApplication>
#include <KTreeWidgetSearchLine>
#include <KMessageBox>

using namespace MessageViewer;
DKIMManagerKeyWidget::DKIMManagerKeyWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mainLayout->setContentsMargins({});

    mTreeWidget = new QTreeWidget(this);
    mTreeWidget->setObjectName(QStringLiteral("treewidget"));
    mTreeWidget->setRootIsDecorated(false);
    mTreeWidget->setHeaderLabels({i18n("SDID"), i18n("Selector"), i18n("DKIM Key")});
    mTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    mTreeWidget->setAlternatingRowColors(true);

    KTreeWidgetSearchLine *searchLineEdit = new KTreeWidgetSearchLine(this, mTreeWidget);
    searchLineEdit->setObjectName(QStringLiteral("searchlineedit"));
    searchLineEdit->setClearButtonEnabled(true);
    mainLayout->addWidget(searchLineEdit);

    mainLayout->addWidget(mTreeWidget);
    connect(mTreeWidget, &QTreeWidget::customContextMenuRequested, this, &DKIMManagerKeyWidget::customContextMenuRequested);
}

DKIMManagerKeyWidget::~DKIMManagerKeyWidget()
{
}

QByteArray DKIMManagerKeyWidget::saveHeaders() const
{
    return mTreeWidget->header()->saveState();
}

void DKIMManagerKeyWidget::restoreHeaders(const QByteArray &header)
{
    mTreeWidget->header()->restoreState(header);
}

void DKIMManagerKeyWidget::customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = mTreeWidget->itemAt(pos);
    QMenu menu(this);
    if (item) {
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-copy")), i18n("Copy Key"), this, [item]() {
            QApplication::clipboard()->setText(item->text(2));
        });
        menu.addSeparator();
        menu.addAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("Remove Key"), this, [this, item]() {
            if (KMessageBox::Yes == KMessageBox::warningYesNo(this, i18n("Do you want to delete this key?"), i18n("Delete Key"))) {
                delete item;
            }
        });
        menu.addSeparator();
    }
    if (mTreeWidget->topLevelItemCount() > 0) {
        menu.addAction(i18n("Delete All"), this, [this]() {
            if (KMessageBox::Yes == KMessageBox::warningYesNo(this, i18n("Do you want to delete all rules?"), i18n("Delete Rules"))) {
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
        QTreeWidgetItem *item = new QTreeWidgetItem(mTreeWidget);
        item->setText(0, key.domain);
        item->setText(1, key.selector);
        item->setText(2, key.keyValue);
        item->setToolTip(2, key.keyValue);
    }
}

void DKIMManagerKeyWidget::saveKeys()
{
    QVector<MessageViewer::KeyInfo> lst;
    lst.reserve(mTreeWidget->topLevelItemCount());
    for (int i = 0, total = mTreeWidget->topLevelItemCount(); i < total; ++i) {
        QTreeWidgetItem *item = mTreeWidget->topLevelItem(i);
        const MessageViewer::KeyInfo info {item->text(2), item->text(1), item->text(0)};
        lst.append(info);
    }
    DKIMManagerKey::self()->saveKeys(lst);
}

void DKIMManagerKeyWidget::resetKeys()
{
    mTreeWidget->clear();
    loadKeys();
}

/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "messageviewer_export.h"
#include <QTreeWidgetItem>
#include <QWidget>
class QTreeWidget;
namespace MessageViewer
{
class DKIMManagerKeyTreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit DKIMManagerKeyTreeWidgetItem(QTreeWidget *parent = nullptr);
    ~DKIMManagerKeyTreeWidgetItem() override;
    Q_REQUIRED_RESULT bool operator<(const QTreeWidgetItem &other) const override;
};

/**
 * @brief The DKIMManagerKeyWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT DKIMManagerKeyWidget : public QWidget
{
    Q_OBJECT
public:
    enum ManagerKeyTreeWidget {
        Domain = 0,
        Selector = 1,
        KeyValue = 2,
        InsertDate = 3,
        LastUsedDate = 3,
    };
    explicit DKIMManagerKeyWidget(QWidget *parent = nullptr);
    ~DKIMManagerKeyWidget() override;

    void loadKeys();
    void saveKeys();
    void resetKeys();
    Q_REQUIRED_RESULT QByteArray saveHeaders() const;
    void restoreHeaders(const QByteArray &header);

private:
    void slotCustomContextMenuRequested(const QPoint &);
    QTreeWidget *const mTreeWidget;
};
}


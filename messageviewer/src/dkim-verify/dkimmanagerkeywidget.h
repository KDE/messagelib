/*
   SPDX-FileCopyrightText: 2018-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once

#include "messageviewer_export.h"
#include <QDateTime>
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
    Q_REQUIRED_RESULT const QDateTime &storedAtDateTime() const;
    void setStoredAtDateTime(const QDateTime &newStoredAtDateTime);

    Q_REQUIRED_RESULT const QDateTime &lastUsedDateTime() const;
    void setLastUsedDateTime(const QDateTime &newLastUsedDateTime);

private:
    QDateTime mStoredAtDateTime;
    QDateTime mLastUsedDateTime;
};

class DKIMManagerKeyTreeView;
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
    MESSAGEVIEWER_NO_EXPORT void slotCustomContextMenuRequested(const QPoint &);
    QTreeWidget *const mTreeWidget;
    DKIMManagerKeyTreeView *mDKIMManagerKeyTreeView = nullptr;
};
}

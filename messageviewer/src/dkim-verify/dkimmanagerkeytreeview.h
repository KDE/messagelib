/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once
#include "dkimmanagerkey.h"
#include "messageviewer_private_export.h"
#include <QTreeView>
namespace MessageViewer
{
class DKIMManagerKeyProxyModel;
class DKIMManagerKeyModel;
class MESSAGEVIEWER_TESTS_EXPORT DKIMManagerKeyTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit DKIMManagerKeyTreeView(QWidget *parent = nullptr);
    ~DKIMManagerKeyTreeView() override;

    void setFilterStr(const QString &str);

    void setKeyModel(DKIMManagerKeyModel *model);

    [[nodiscard]] QList<MessageViewer::KeyInfo> keyInfos() const;

    void clear();

private:
    MESSAGEVIEWER_NO_EXPORT void deleteSelectedItems();
    MESSAGEVIEWER_NO_EXPORT void slotCustomContextMenuRequested(const QPoint &pos);
    DKIMManagerKeyProxyModel *const mManagerKeyProxyModel;
    DKIMManagerKeyModel *mManagerKeyModel = nullptr;
};
}

/*
   SPDX-FileCopyrightText: 2023-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once
#include "messageviewer_private_export.h"
#include <MessageCore/DKIMManagerKey>
#include <QTreeView>
namespace MessageCore
{
class DKIMManagerKeyProxyModel;
class DKIMManagerKeyModel;
}
namespace MessageViewer
{
class MESSAGEVIEWER_TESTS_EXPORT DKIMManagerKeyTreeView : public QTreeView
{
    Q_OBJECT
public:
    explicit DKIMManagerKeyTreeView(QWidget *parent = nullptr);
    ~DKIMManagerKeyTreeView() override;

    void setFilterStr(const QString &str);

    void setKeyModel(MessageCore::DKIMManagerKeyModel *model);

    [[nodiscard]] QList<MessageCore::KeyInfo> keyInfos() const;

    void clear();

private:
    MESSAGEVIEWER_NO_EXPORT void deleteSelectedItems();
    MESSAGEVIEWER_NO_EXPORT void slotCustomContextMenuRequested(const QPoint &pos);
    MessageCore::DKIMManagerKeyProxyModel *const mManagerKeyProxyModel;
    MessageCore::DKIMManagerKeyModel *mManagerKeyModel = nullptr;
};
}

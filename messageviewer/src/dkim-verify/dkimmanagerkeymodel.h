/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#pragma once
#include "dkimmanagerkey.h"
#include "messageviewer_export.h"
#include <QAbstractListModel>
#include <QList>
namespace MessageViewer
{
class MESSAGEVIEWER_EXPORT DKIMManagerKeyModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DKIMManagerKeyRoles {
        DomainRole,
        SelectorRole,
        KeyRole,
        StoredAtDateTimeRole,
        LastUsedDateTimeRole,
        LastColumn = LastUsedDateTimeRole,
    };

    explicit DKIMManagerKeyModel(QObject *parent = nullptr);
    ~DKIMManagerKeyModel() override;

    [[nodiscard]] QList<MessageViewer::KeyInfo> keyInfos() const;
    void setKeyInfos(const QList<MessageViewer::KeyInfo> &newKeyInfos);

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    [[nodiscard]] QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    [[nodiscard]] bool insertKeyInfo(const MessageViewer::KeyInfo &keyInfo);
    void removeKeyInfo(const QString &keyValue);
    void removeKeyInfos(const QStringList &keyInfos);

    void clear();

private:
    QList<MessageViewer::KeyInfo> mKeyInfos;
};
}

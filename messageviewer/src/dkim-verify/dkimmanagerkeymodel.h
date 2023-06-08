/*
   SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

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
        KeyRole,
        SelectorRole,
        DomainRole,
        StoredAtDateTimeRole,
        LastUsedDateTimeRole,
        LastColumn = LastUsedDateTimeRole,
    };

    explicit DKIMManagerKeyModel(QObject *parent = nullptr);
    ~DKIMManagerKeyModel() override;

    Q_REQUIRED_RESULT QList<MessageViewer::KeyInfo> keyInfos() const;
    void setKeyInfos(const QList<MessageViewer::KeyInfo> &newKeyInfos);

    Q_REQUIRED_RESULT int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_REQUIRED_RESULT int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    Q_REQUIRED_RESULT QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_REQUIRED_RESULT bool insertKeyInfo(const MessageViewer::KeyInfo &keyInfo);
    void removeKeyInfo(const MessageViewer::KeyInfo &keyInfo);

    void clear();

private:
    QList<MessageViewer::KeyInfo> mKeyInfos;
};
}

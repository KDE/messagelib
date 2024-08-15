/*
   SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "dkimmanagerkeymodel.h"
#include <KLocalizedString>
using namespace MessageViewer;

DKIMManagerKeyModel::DKIMManagerKeyModel(QObject *parent)
    : QAbstractListModel{parent}
{
}

DKIMManagerKeyModel::~DKIMManagerKeyModel() = default;

QList<MessageViewer::KeyInfo> DKIMManagerKeyModel::keyInfos() const
{
    return mKeyInfos;
}

void DKIMManagerKeyModel::setKeyInfos(const QList<MessageViewer::KeyInfo> &newKeyInfos)
{
    beginResetModel();
    mKeyInfos = newKeyInfos;
    endResetModel();
}

int DKIMManagerKeyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0; // flat model
    }
    return mKeyInfos.count();
}

int DKIMManagerKeyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return static_cast<int>(DKIMManagerKeyRoles::LastColumn) + 1;
}

QVariant DKIMManagerKeyModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mKeyInfos.count()) {
        return {};
    }
    const KeyInfo &keyInfo = mKeyInfos.at(index.row());
    if (role != Qt::DisplayRole) {
        return {};
    }
    switch (static_cast<DKIMManagerKeyRoles>(index.column())) {
    case KeyRole: {
        return keyInfo.keyValue;
    }
    case SelectorRole:
        return keyInfo.selector;
    case DomainRole:
        return keyInfo.domain;
    case StoredAtDateTimeRole:
        return QLocale().toString(keyInfo.storedAtDateTime);
    case LastUsedDateTimeRole:
        return QLocale().toString(keyInfo.lastUsedDateTime);
    }
    return {};
}

QVariant DKIMManagerKeyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return {};
    }

    switch (static_cast<DKIMManagerKeyRoles>(section)) {
    case KeyRole:
        return i18n("DKIM Key");
    case SelectorRole:
        return i18n("Selector");
    case DomainRole:
        return i18n("SDID");
    case StoredAtDateTimeRole:
        return i18n("Inserted");
    case LastUsedDateTimeRole:
        return i18n("Last Used");
    }
    return {};
}

void DKIMManagerKeyModel::clear()
{
    if (!mKeyInfos.isEmpty()) {
        beginResetModel();
        mKeyInfos.clear();
        endResetModel();
    }
}

bool DKIMManagerKeyModel::insertKeyInfo(const KeyInfo &keyInfo)
{
    bool found = false;
    auto it = std::find_if(mKeyInfos.cbegin(), mKeyInfos.cend(), [keyInfo](const KeyInfo &key) {
        return key == keyInfo;
    });
    if (it == mKeyInfos.cend()) {
        beginInsertRows(QModelIndex(), mKeyInfos.count() - 1, mKeyInfos.count());
        mKeyInfos.append(keyInfo);
        endInsertRows();
    } else {
        found = true;
    }
    return found;
}

void DKIMManagerKeyModel::removeKeyInfo(const QString &keyValue)
{
    auto it = std::find_if(mKeyInfos.cbegin(), mKeyInfos.cend(), [keyValue](const KeyInfo &key) {
        return key.keyValue == keyValue;
    });
    if (it != mKeyInfos.cend()) {
        beginResetModel();
        mKeyInfos.removeAll(*it);
        endResetModel();
    }
}

void DKIMManagerKeyModel::removeKeyInfos(const QStringList &keyInfos)
{
    if (keyInfos.isEmpty()) {
        return;
    }
    beginResetModel();
    for (const auto &keyInfo : keyInfos) {
        auto it = std::find_if(mKeyInfos.cbegin(), mKeyInfos.cend(), [keyInfo](const KeyInfo &key) {
            return key.keyValue == keyInfo;
        });
        if (it != mKeyInfos.cend()) {
            mKeyInfos.removeAll(*it);
        }
    }
    endResetModel();
}

#include "moc_dkimmanagerkeymodel.cpp"

/*
    SPDX-FileCopyrightText: 2025 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagelist_export.h"
#include <Akonadi/Tag>
#include <QMap>
#include <QObject>
namespace Akonadi
{
class Monitor;
}
class KJob;
namespace MessageList
{
namespace Core
{
class MESSAGELIST_EXPORT TagManager : public QObject
{
    Q_OBJECT
public:
    explicit TagManager(QObject *parent = nullptr);
    ~TagManager() override;

    static TagManager *self();

    [[nodiscard]] QMap<QString, QString> mapTag() const;
    void setMapTag(const QMap<QString, QString> &newMapTag);

    [[nodiscard]] QString tagFromName(const QString &name) const;

Q_SIGNALS:
    void tagsFetched(const Akonadi::Tag::List &lst);

private:
    void init();
    void slotTagsFetched(KJob *job);
    void slotTagsChanged();
    Akonadi::Monitor *const mMonitor;
    QMap<QString, QString> mMapTag;
};
}
}

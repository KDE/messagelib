/*
    SPDX-FileCopyrightText: 2025 Laurent Montel <montel@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>
namespace Akonadi
{
class Monitor;
}
namespace MessageList
{
namespace Core
{
class TagManager : public QObject
{
    Q_OBJECT
public:
    explicit TagManager(QObject *parent = nullptr);
    ~TagManager() override;

    static TagManager *self();

Q_SIGNALS:
    void tagsChanged();

private:
    void init();
    void slotTagsChanged();
    Akonadi::Monitor *const mMonitor;
};
}
}

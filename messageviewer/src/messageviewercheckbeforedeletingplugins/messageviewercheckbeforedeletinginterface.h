/*
   SPDX-FileCopyrightText: 2021-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <Akonadi/Item>
#include <QList>
#include <QObject>
class QAction;
class KActionCollection;
namespace MessageViewer
{
class MessageViewerCheckBeforeDeletingParameters;
class MessageViewerCheckBeforeDeletingInterfacePrivate;
/**
 * @brief The MessageViewerCheckBeforeDeletingInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MessageViewerCheckBeforeDeletingInterface : public QObject
{
    Q_OBJECT
public:
    explicit MessageViewerCheckBeforeDeletingInterface(QObject *parent = nullptr);
    ~MessageViewerCheckBeforeDeletingInterface() override;

    void setParentWidget(QWidget *parent);
    [[nodiscard]] QWidget *parentWidget() const;

    [[nodiscard]] virtual Akonadi::Item::List exec(const Akonadi::Item::List &list) = 0;

    void setParameters(const MessageViewer::MessageViewerCheckBeforeDeletingParameters &params);
    [[nodiscard]] MessageViewer::MessageViewerCheckBeforeDeletingParameters parameters() const;

    [[nodiscard]] virtual QList<QAction *> actions() const;

    virtual void createActions(KActionCollection *ac);

Q_SIGNALS:
    void configure(QWidget *parent);

public Q_SLOTS:
    virtual void reloadConfig();

private:
    std::unique_ptr<MessageViewerCheckBeforeDeletingInterfacePrivate> const d;
};
}

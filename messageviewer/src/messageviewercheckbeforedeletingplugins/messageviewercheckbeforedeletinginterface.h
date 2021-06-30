/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <AkonadiCore/Item>
#include <QObject>
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
    Q_REQUIRED_RESULT QWidget *parentWidget() const;

    virtual Q_REQUIRED_RESULT Akonadi::Item::List exec(const Akonadi::Item::List &list) = 0;

    void setParameters(const MessageViewer::MessageViewerCheckBeforeDeletingParameters &params);
    Q_REQUIRED_RESULT MessageViewer::MessageViewerCheckBeforeDeletingParameters parameters() const;

public Q_SLOTS:
    virtual void reloadConfig();

private:
    MessageViewerCheckBeforeDeletingInterfacePrivate *const d;
};
}

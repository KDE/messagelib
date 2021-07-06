/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

#include "messageviewer_export.h"
namespace MessageViewer
{
class MessageViewerCheckBeforeDeletingInterface;
class MessageViewerCheckBeforeDeletingPluginPrivate;
/**
 * @brief The MessageViewerCheckBeforeDeletingPlugin class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MessageViewerCheckBeforeDeletingPlugin : public QObject
{
    Q_OBJECT
public:
    explicit MessageViewerCheckBeforeDeletingPlugin(QObject *parent = nullptr);
    ~MessageViewerCheckBeforeDeletingPlugin();

    virtual MessageViewerCheckBeforeDeletingInterface *createInterface(QObject *parent) = 0;
    virtual void showConfigureDialog(QWidget *parent);

    void setIsEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isEnabled() const;
Q_SIGNALS:
    void configChanged();

private:
    MessageViewerCheckBeforeDeletingPluginPrivate *const d;
};
}

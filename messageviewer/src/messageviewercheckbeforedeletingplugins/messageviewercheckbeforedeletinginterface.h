/*
   SPDX-FileCopyrightText: 2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include "plugineditorcheckbeforesendparams.h"
#include <QObject>

namespace MessageViewer
{
class MessageViewerCheckBeforeDeletingInterfacePrivate;
/**
 * @brief The MessageViewerCheckBeforeDeletingInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT MessageViewerCheckBeforeDeletingInterface : public QObject
{
    Q_OBJECT
public:
    explicit MessageViewerCheckBeforeDeletingInterface(QObject *parent = nullptr);
    ~MessageViewerCheckBeforeDeletingInterface() override;

    void setParentWidget(QWidget *parent);
    Q_REQUIRED_RESULT QWidget *parentWidget() const;

    // TODO add virtual method

public Q_SLOTS:
    virtual void reloadConfig();

private:
    MessageViewerCheckBeforeDeletingInterfacePrivate *const d;
};
}

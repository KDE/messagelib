/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include "plugineditorcheckbeforesendparams.h"
#include <QObject>

namespace MessageComposer
{
class PluginEditorCheckBeforeSendInterfacePrivate;
class PluginEditorCheckBeforeSendParams;
/**
 * @brief The PluginEditorCheckBeforeSendInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorCheckBeforeSendInterface : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorCheckBeforeSendInterface(QObject *parent = nullptr);
    ~PluginEditorCheckBeforeSendInterface() override;

    virtual bool exec(const MessageComposer::PluginEditorCheckBeforeSendParams &params) = 0;

    void setParentWidget(QWidget *parent);
    Q_REQUIRED_RESULT QWidget *parentWidget() const;

    void setParameters(const MessageComposer::PluginEditorCheckBeforeSendParams &params);
    Q_REQUIRED_RESULT MessageComposer::PluginEditorCheckBeforeSendParams parameters() const;

public Q_SLOTS:
    virtual void reloadConfig();

private:
    PluginEditorCheckBeforeSendInterfacePrivate *const d;
};
}


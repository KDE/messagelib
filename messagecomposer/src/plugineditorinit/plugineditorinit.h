/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <MessageComposer/PluginEditorBase>
#include <QObject>

namespace MessageComposer
{
class PluginEditorInitInterface;
/**
 * @brief The PluginEditorInit class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorInit : public PluginEditorBase
{
    Q_OBJECT
public:
    explicit PluginEditorInit(QObject *parent = nullptr);
    ~PluginEditorInit() override;

    virtual PluginEditorInitInterface *createInterface(QObject *parent) = 0;
};
}

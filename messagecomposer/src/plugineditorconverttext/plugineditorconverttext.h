/*
   SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <MessageComposer/PluginEditorBase>
namespace MessageComposer
{
class PluginEditorConvertTextInterface;
/**
 * @brief The PluginEditorConvertText class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConvertText : public PluginEditorBase
{
    Q_OBJECT
public:
    explicit PluginEditorConvertText(QObject *parent = nullptr);
    ~PluginEditorConvertText() override;

    virtual PluginEditorConvertTextInterface *createInterface(QObject *parent) = 0;

    Q_REQUIRED_RESULT virtual bool canWorkOnHtml() const;

    Q_REQUIRED_RESULT virtual bool hasStatusBarSupport() const;

    Q_REQUIRED_RESULT virtual bool hasPopupMenuSupport() const;

    Q_REQUIRED_RESULT virtual bool hasToolBarSupport() const;
};
}

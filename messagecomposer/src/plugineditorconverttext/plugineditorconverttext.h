/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <MessageComposer/PluginEditorBase>
namespace MessageComposer
{
class PluginEditorConvertTextInterface;
/*!
 \class MessageComposer::PluginEditorConvertText
 \inmodule MessageComposer
 \inheaderfile MessageComposer/PluginEditorConvertText
 \author Laurent Montel <montel@kde.org>

 The PluginEditorConvertText class.
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConvertText : public PluginEditorBase
{
    Q_OBJECT
public:
    /*!
     */
    explicit PluginEditorConvertText(QObject *parent = nullptr);
    /*!
     */
    ~PluginEditorConvertText() override;

    /*!
     */
    [[nodiscard]] virtual PluginEditorConvertTextInterface *createInterface(QObject *parent) = 0;

    /*!
     */
    [[nodiscard]] virtual bool canWorkOnHtml() const;

    /*!
     */
    [[nodiscard]] virtual bool hasStatusBarSupport() const;

    /*!
     */
    [[nodiscard]] virtual bool hasPopupMenuSupport() const;

    /*!
     */
    [[nodiscard]] virtual bool hasToolBarSupport() const;
};
}

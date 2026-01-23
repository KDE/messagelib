/*
   SPDX-FileCopyrightText: 2015-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messagecomposer_export.h"
#include <PimCommon/AbstractGenericPlugin>
#include <QObject>
#include <memory>
namespace MessageComposer
{
class PluginEditorPrivate;
/*!
 \class MessageComposer::PluginEditor
 \inmodule MessageComposer
 \inheaderfile MessageComposer/PluginEditor
 \author Laurent Montel <montel@kde.org>

 The PluginEditor class.
 */
class MESSAGECOMPOSER_EXPORT PluginEditor : public PimCommon::AbstractGenericPlugin
{
    Q_OBJECT
public:
    /*!
     */
    explicit PluginEditor(QObject *parent = nullptr);
    /*!
     */
    ~PluginEditor() override;
    /*!
     */
    void setOrder(int order);
    /*!
     */
    [[nodiscard]] int order() const;

    /*!
     */
    [[nodiscard]] virtual bool canProcessKeyEvent() const;

private:
    std::unique_ptr<PluginEditorPrivate> const d;
};
}

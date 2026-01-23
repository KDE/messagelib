/*
   SPDX-FileCopyrightText: 2021-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QObject>
#include <memory>
namespace MessageComposer
{
class PluginEditorBasePrivate;
/*!
 \class MessageComposer::PluginEditorBase
 \inmodule MessageComposer
 \inheaderfile MessageComposer/PluginEditorBase
 \author Laurent Montel <montel@kde.org>

 The PluginEditorBase class.
 */
class MESSAGECOMPOSER_EXPORT PluginEditorBase : public QObject
{
    Q_OBJECT
public:
    /*!
     */
    explicit PluginEditorBase(QObject *parent = nullptr);
    /*!
     */
    ~PluginEditorBase() override;

    /*!
     */
    [[nodiscard]] virtual bool hasConfigureDialog() const;

    /*!
     */
    virtual void showConfigureDialog(QWidget *parent = nullptr);

    /*!
     */
    void emitConfigChanged();

    /*!
     */
    [[nodiscard]] virtual QString description() const;

    /*!
     */
    void setIsEnabled(bool enabled);
    /*!
     */
    [[nodiscard]] bool isEnabled() const;

Q_SIGNALS:
    /*!
     */
    void configChanged();

private:
    std::unique_ptr<PluginEditorBasePrivate> const d;
};
}

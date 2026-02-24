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
    /*! \brief Constructs a PluginEditorBase.
        \param parent The parent object.
    */
    explicit PluginEditorBase(QObject *parent = nullptr);
    /*! \brief Destroys the PluginEditorBase. */
    ~PluginEditorBase() override;

    /*! \brief Returns whether this plugin has a configuration dialog. */
    [[nodiscard]] virtual bool hasConfigureDialog() const;

    /*! \brief Shows the configuration dialog for this plugin.
        \param parent The parent widget for the dialog.
    */
    virtual void showConfigureDialog(QWidget *parent = nullptr);

    /*! \brief Emits the configChanged signal. */
    void emitConfigChanged();

    /*! \brief Returns a human-readable description of the plugin. */
    [[nodiscard]] virtual QString description() const;

    /*! \brief Enables or disables the plugin.
        \param enabled True to enable, false to disable.
    */
    void setIsEnabled(bool enabled);
    /*! \brief Returns whether the plugin is enabled. */
    [[nodiscard]] bool isEnabled() const;

Q_SIGNALS:
    /*! \brief Emitted when the plugin configuration has changed. */
    void configChanged();

private:
    std::unique_ptr<PluginEditorBasePrivate> const d;
};
}

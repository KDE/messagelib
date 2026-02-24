/*
   SPDX-FileCopyrightText: 2016-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QWidget>

namespace MessageComposer
{
/*!
 \class MessageComposer::PluginEditorConfigureBaseWidget
 \inmodule MessageComposer
 \inheaderfile MessageComposer/PluginEditorConfigureBaseWidget
 \author Laurent Montel <montel@kde.org>

 The PluginEditorConfigureBaseWidget class.
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConfigureBaseWidget : public QWidget
{
    Q_OBJECT
public:
    /*! \brief Constructs a PluginEditorConfigureBaseWidget.
        \param parent The parent widget.
    */
    explicit PluginEditorConfigureBaseWidget(QWidget *parent = nullptr);
    /*! \brief Destroys the PluginEditorConfigureBaseWidget. */
    ~PluginEditorConfigureBaseWidget() override;

    /*! \brief Loads the plugin settings from the configuration. */
    virtual void loadSettings() = 0;
    /*! \brief Saves the plugin settings to the configuration. */
    virtual void saveSettings() = 0;
    /*! \brief Resets all settings to their default values. */
    virtual void resetSettings() = 0;
    /*! \brief Returns the help anchor for this configuration widget. */
    virtual QString helpAnchor() const;
Q_SIGNALS:
    /*! \brief Emitted when the plugin configuration changes. */
    void configureChanged();
};
}

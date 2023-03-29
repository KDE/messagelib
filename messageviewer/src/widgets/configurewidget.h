/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <QWidget>

class KConfigDialogManager;

namespace MessageViewer
{
class ConfigureWidgetPrivate;
/**
 * Configure widget that can be used in a KConfigDialog.
 *
 * @author Andras Mantia <andras@kdab.net>
 */
class MESSAGEVIEWER_EXPORT ConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigureWidget(QWidget *parent = nullptr);

    ~ConfigureWidget() override;

    //
    // Read and write config settings to the GlobalSettings.
    // Note that this does not deal with all settings, some of those settings need to saved and read
    // with a KConfigDialogManager, since this widgets correctly sets the objectname to the pattern
    // required by KConfigDialogManager.
    //

    void writeConfig();
    void readConfig();

Q_SIGNALS:

    /**
     * Emitted when the user changes the setting in some widget. Useful to enable the "Apply"
     * button after this has been emitted.
     */
    void settingsChanged();

private:
    MESSAGEVIEWER_NO_EXPORT void readCurrentOverrideCodec();
    std::unique_ptr<ConfigureWidgetPrivate> const d;
};
}

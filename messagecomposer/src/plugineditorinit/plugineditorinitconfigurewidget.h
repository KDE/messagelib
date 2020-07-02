/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PluginEditorInitConfigureWidget_H
#define PluginEditorInitConfigureWidget_H

#include "messagecomposer_export.h"
#include <QWidget>

namespace MessageComposer {
/**
 * @brief The PluginEditorInitConfigureWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorInitConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PluginEditorInitConfigureWidget(QWidget *parent = nullptr);
    ~PluginEditorInitConfigureWidget();

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
    virtual void resetSettings() = 0;
    Q_REQUIRED_RESULT virtual QString helpAnchor() const;
Q_SIGNALS:
    void configureChanged();
};
}
#endif // PluginEditorInitConfigureWidget_H

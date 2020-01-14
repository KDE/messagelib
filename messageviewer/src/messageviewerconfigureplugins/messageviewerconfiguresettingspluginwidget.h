/*
   Copyright (C) 2018-2020 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef MESSAGEVIEWERCONFIGURESETTINGSPLUGINWIDGET_H
#define MESSAGEVIEWERCONFIGURESETTINGSPLUGINWIDGET_H

#include "messageviewer_export.h"
#include <QWidget>

namespace MessageViewer {
/**
 * @brief The MessageViewerConfigureSettingsPluginWidget class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT MessageViewerConfigureSettingsPluginWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MessageViewerConfigureSettingsPluginWidget(QWidget *parent = nullptr);
    ~MessageViewerConfigureSettingsPluginWidget();

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
    virtual void resetSettings() = 0;
    virtual Q_REQUIRED_RESULT QString helpAnchor() const;
Q_SIGNALS:
    void configureChanged();
};
}
#endif // MESSAGEVIEWERCONFIGURESETTINGSPLUGINWIDGET_H

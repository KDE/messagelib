/*
   Copyright (C) 2017-2020 Laurent Montel <montel@kde.org>

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

/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#ifndef NETWORKPLUGINURLINTERCEPTORCONFIGUREWIDGET_H
#define NETWORKPLUGINURLINTERCEPTORCONFIGUREWIDGET_H

#include <QWidget>
#include "webengineviewer_export.h"

namespace WebEngineViewer
{
class WEBENGINEVIEWER_EXPORT NetworkPluginUrlInterceptorConfigureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NetworkPluginUrlInterceptorConfigureWidget(QWidget *parent = nullptr);
    ~NetworkPluginUrlInterceptorConfigureWidget();

    virtual void loadSettings() = 0;
    virtual void saveSettings() = 0;
    virtual void resetSettings() = 0;

    virtual QString helpAnchor() const;

Q_SIGNALS:
    void configureChanged();
};
}
#endif // NETWORKPLUGINURLINTERCEPTORCONFIGUREWIDGET_H

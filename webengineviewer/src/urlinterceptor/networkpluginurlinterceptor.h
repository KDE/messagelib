/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "webengineviewer_export.h"
#include <QObject>
class QWebEngineView;
namespace WebEngineViewer
{
class NetworkPluginUrlInterceptorInterface;
/**
 * @brief The NetworkPluginUrlInterceptor class
 * @author Laurent Montel <montel@kde.org>
 */
class WEBENGINEVIEWER_EXPORT NetworkPluginUrlInterceptor : public QObject
{
    Q_OBJECT
public:
    explicit NetworkPluginUrlInterceptor(QObject *parent = nullptr);
    ~NetworkPluginUrlInterceptor() override;
    virtual NetworkPluginUrlInterceptorInterface *createInterface(QWebEngineView *webEngine, QObject *parent = nullptr) = 0;

    virtual Q_REQUIRED_RESULT bool hasConfigureDialog() const;
    virtual void showConfigureDialog(QWidget *parent = nullptr);

    void setIsEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isEnabled() const;

private:
    bool mIsEnabled = false;
};
}


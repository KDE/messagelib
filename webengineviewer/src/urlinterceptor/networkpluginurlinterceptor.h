/*
   SPDX-FileCopyrightText: 2016-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MAILNETWORKPLUGINURLINTERCEPTOR_H
#define MAILNETWORKPLUGINURLINTERCEPTOR_H

#include <QObject>
#include "webengineviewer_export.h"
class QWebEngineView;
namespace WebEngineViewer {
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
    ~NetworkPluginUrlInterceptor();
    virtual NetworkPluginUrlInterceptorInterface *createInterface(QWebEngineView *webEngine, QObject *parent = nullptr) = 0;

    virtual bool hasConfigureDialog() const;
    virtual void showConfigureDialog(QWidget *parent = nullptr);

    void setIsEnabled(bool enabled);
    Q_REQUIRED_RESULT bool isEnabled() const;

private:
    bool mIsEnabled = false;
};
}

#endif // MAILNETWORKPLUGINURLINTERCEPTOR_H

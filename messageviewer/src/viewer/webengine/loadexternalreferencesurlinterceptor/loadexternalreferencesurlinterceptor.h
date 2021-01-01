/*
   SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#ifndef LOADEXTERNALREFERENCESURLINTERCEPTOR_H
#define LOADEXTERNALREFERENCESURLINTERCEPTOR_H

#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>

namespace MessageViewer {
class LoadExternalReferencesUrlInterceptor : public WebEngineViewer::NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    explicit LoadExternalReferencesUrlInterceptor(QObject *parent = nullptr);
    ~LoadExternalReferencesUrlInterceptor() override;

    Q_REQUIRED_RESULT bool interceptRequest(QWebEngineUrlRequestInfo &info) override;
    void setAllowExternalContent(bool b);
    Q_REQUIRED_RESULT bool allowExternalContent() const;
private:
    bool mAllowLoadExternalReference = false;
};
}
#endif // LOADEXTERNALREFERENCESURLINTERCEPTOR_H

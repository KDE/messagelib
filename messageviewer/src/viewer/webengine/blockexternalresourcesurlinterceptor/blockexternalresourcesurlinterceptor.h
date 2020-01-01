/*
   Copyright (C) 2016-2020 Laurent Montel <montel@kde.org>

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
#ifndef BLOCKEXTERNALRESOURCESURLINTERCEPTOR_H
#define BLOCKEXTERNALRESOURCESURLINTERCEPTOR_H

#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>

namespace MessageViewer {
class BlockExternalResourcesUrlInterceptor : public WebEngineViewer::
    NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    explicit BlockExternalResourcesUrlInterceptor(QObject *parent = nullptr);
    ~BlockExternalResourcesUrlInterceptor() override;

    Q_REQUIRED_RESULT bool interceptRequest(QWebEngineUrlRequestInfo &info) override;
    void setAllowExternalContent(bool b);
    Q_REQUIRED_RESULT bool allowExternalContent() const;
Q_SIGNALS:
    void formSubmittedForbidden();
};
}
#endif // BLOCKEXTERNALRESOURCESURLINTERCEPTOR_H

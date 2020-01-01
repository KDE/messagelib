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

#ifndef BLOCKMAILTRACKINGURLINTERCEPTOR_H
#define BLOCKMAILTRACKINGURLINTERCEPTOR_H

#include <WebEngineViewer/NetworkPluginUrlInterceptorInterface>
#include "messageviewer_export.h"
#include <QVector>
namespace MessageViewer {
/**
 * @brief The BlockMailTrackingUrlInterceptor class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT BlockMailTrackingUrlInterceptor : public WebEngineViewer::NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    struct MailTrackerBlackList
    {
        MailTrackerBlackList() = default;
        MailTrackerBlackList(const QString &company, const QString &pattern, const QString &url)
            : mCompanyName(company)
            , mCompanyUrl(url)
            , mPattern(pattern)
        {
        }

        QString mCompanyName;
        QString mCompanyUrl;
        QString mPattern;
    };

    explicit BlockMailTrackingUrlInterceptor(QObject *parent = nullptr);
    ~BlockMailTrackingUrlInterceptor() override;
    Q_REQUIRED_RESULT bool interceptRequest(QWebEngineUrlRequestInfo &info) override;

Q_SIGNALS:
    void mailTrackingFound(const MessageViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList &);

private:
    void initializeList();
    QVector<MailTrackerBlackList> mBackList;
};
}
Q_DECLARE_TYPEINFO(MessageViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(MessageViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList)
#endif // BLOCKMAILTRACKINGURLINTERCEPTOR_H

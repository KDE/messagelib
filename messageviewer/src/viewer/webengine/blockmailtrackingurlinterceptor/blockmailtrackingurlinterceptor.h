/*
   Copyright (C) 2017-2018 Laurent Montel <montel@kde.org>

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

#include <QVector>
namespace MessageViewer {
class BlockMailTrackingUrlInterceptor : public WebEngineViewer::NetworkPluginUrlInterceptorInterface
{
    Q_OBJECT
public:
    struct MailTrackerBlackList
    {
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
    ~BlockMailTrackingUrlInterceptor();
    bool interceptRequest(QWebEngineUrlRequestInfo &info) override;

Q_SIGNALS:
    void mailTrackingFound(const MessageViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList &);

private:
    void initializeList();
    QVector<MailTrackerBlackList> mBackList;
};
}
Q_DECLARE_TYPEINFO(MessageViewer::BlockMailTrackingUrlInterceptor::MailTrackerBlackList, Q_MOVABLE_TYPE);
#endif // BLOCKMAILTRACKINGURLINTERCEPTOR_H

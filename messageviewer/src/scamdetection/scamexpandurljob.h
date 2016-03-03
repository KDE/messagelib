/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef SCAMEXPANDURLJOB_H
#define SCAMEXPANDURLJOB_H

#include <QObject>
#include <QNetworkReply>
class QNetworkAccessManager;
class QNetworkConfigurationManager;
namespace MessageViewer
{
class ScamExpandUrlJob : public QObject
{
    Q_OBJECT
public:
    explicit ScamExpandUrlJob(QObject *parent = Q_NULLPTR);
    ~ScamExpandUrlJob();

    void expandedUrl(const QUrl &url);
Q_SIGNALS:
    void urlExpanded(const QString &shortUrl, const QString &expandedUrl);
    void expandUrlError(QNetworkReply::NetworkError error);

private Q_SLOTS:
    void slotError(QNetworkReply::NetworkError error);
    void slotExpandFinished(QNetworkReply *reply);

private:
    QNetworkAccessManager *mNetworkAccessManager;
    QNetworkConfigurationManager *mNetworkConfigurationManager;
};
}
#endif // SCAMEXPANDURLJOB_H

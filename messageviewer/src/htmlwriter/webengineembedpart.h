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

#ifndef WEBENGINEEMBEDPART_H
#define WEBENGINEEMBEDPART_H

#include <QObject>
#include <QMap>
namespace MessageViewer
{
class WebEngineEmbedPart : public QObject
{
    Q_OBJECT
public:
    explicit WebEngineEmbedPart(QObject *parent = Q_NULLPTR);
    ~WebEngineEmbedPart();

    void clear();
    bool isEmpty() const;

    QMap<QString, QString> embeddedPartMap() const;

    void addEmbedPart(const QByteArray &contentId, const QString &contentURL);
    QString contentUrl(const QString &contentId) const;

    static WebEngineEmbedPart *self();
private:
    // Key is Content-Id, value is URL
    QMap<QString, QString> mEmbeddedPartMap;
};
}
#endif // WEBENGINEEMBEDPART_H

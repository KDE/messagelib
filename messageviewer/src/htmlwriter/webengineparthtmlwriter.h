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
#ifndef WEBENGINEPARTHTMLWRITER_H
#define WEBENGINEPARTHTMLWRITER_H

#include "messageviewer/htmlwriter.h"
#include <QObject>

#include <QString>
#include <QByteArray>
#include <QMap>

namespace MessageViewer
{
class MailWebEngineView;
}

namespace MessageViewer
{

class WebEnginePartHtmlWriter : public QObject, public HtmlWriter
{
    Q_OBJECT
public:
    explicit WebEnginePartHtmlWriter(MailWebEngineView *view, QObject *parent = Q_NULLPTR);
    ~WebEnginePartHtmlWriter();

    void begin(const QString &cssDefs) Q_DECL_OVERRIDE;
    void end() Q_DECL_OVERRIDE;
    void reset() Q_DECL_OVERRIDE;
    void write(const QString &str) Q_DECL_OVERRIDE;
    void queue(const QString &str) Q_DECL_OVERRIDE;
    void flush() Q_DECL_OVERRIDE;
    void embedPart(const QByteArray &contentId, const QString &url) Q_DECL_OVERRIDE;
    void extraHead(const QString &str) Q_DECL_OVERRIDE;
Q_SIGNALS:
    void finished();

private:
    void resolveCidUrls();
    void insertExtraHead();

private:
    MailWebEngineView *mHtmlView;
    QString mHtml;
    QString mExtraHead;
    enum State {
        Begun,
        Queued,
        Ended
    } mState;
    // Key is Content-Id, value is URL
    typedef QMap<QString, QString> EmbeddedPartMap;
    EmbeddedPartMap mEmbeddedPartMap;
};

}
#endif // WEBENGINEPARTHTMLWRITER_H

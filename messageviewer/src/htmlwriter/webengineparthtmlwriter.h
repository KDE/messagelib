/*
  Copyright (c) 2016-2018 Montel Laurent <montel@kde.org>

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

#include "bufferedhtmlwriter.h"
#include <QObject>

#include <QString>
#include <QByteArray>

namespace MessageViewer {
class MailWebEngineView;
}

namespace MessageViewer {
class WebEnginePartHtmlWriter : public QObject, public BufferedHtmlWriter
{
    Q_OBJECT
public:
    explicit WebEnginePartHtmlWriter(MailWebEngineView *view, QObject *parent = nullptr);
    ~WebEnginePartHtmlWriter();

    void begin() override;
    void end() override;
    void reset() override;
    void embedPart(const QByteArray &contentId, const QString &url) override;
    void extraHead(const QString &str) override;

Q_SIGNALS:
    void finished();

private:
    void insertExtraHead();

private:
    MailWebEngineView *mHtmlView = nullptr;
    QString mExtraHead;
    enum State {
        Begun,
        Queued,
        Ended
    } mState;
};
}
#endif // WEBENGINEPARTHTMLWRITER_H

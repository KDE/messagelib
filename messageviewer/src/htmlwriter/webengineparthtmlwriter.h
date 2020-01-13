/*
  Copyright (c) 2016-2020 Laurent Montel <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

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
class QTemporaryFile;
namespace MessageViewer {
class MailWebEngineView;
}

namespace MessageViewer {
class WebEnginePartHtmlWriter : public QObject, public BufferedHtmlWriter
{
    Q_OBJECT
public:
    explicit WebEnginePartHtmlWriter(MailWebEngineView *view, QObject *parent = nullptr);
    ~WebEnginePartHtmlWriter() override;

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
    } mState = Ended;
    QTemporaryFile *mTempFile = nullptr;
};
}
#endif // WEBENGINEPARTHTMLWRITER_H

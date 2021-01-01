/*
  SPDX-FileCopyrightText: 2016-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
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
    void setExtraHead(const QString &str) override;

    void setStyleBody(const QString &styleBody) override;

Q_SIGNALS:
    void finished();

private:
    void insertExtraHead();
    void insertBodyStyle();

private:
    MailWebEngineView *const mHtmlView;
    QString mExtraHead;
    QString mStyleBody;
    enum State {
        Begun,
        Queued,
        Ended
    } mState = Ended;
    QTemporaryFile *mTempFile = nullptr;
};
}
#endif // WEBENGINEPARTHTMLWRITER_H

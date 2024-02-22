/*
  SPDX-FileCopyrightText: 2016-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "bufferedhtmlwriter.h"
#include <QObject>

#include <QByteArray>
#include <QString>
class QTemporaryFile;
namespace MessageViewer
{
class MailWebEngineView;
}

namespace MessageViewer
{
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

    MailWebEngineView *const mHtmlView;
    QString mExtraHead;
    QString mStyleBody;
    enum State { Begun, Queued, Ended } mState = Ended;
    QTemporaryFile *mTempFile = nullptr;
};
}

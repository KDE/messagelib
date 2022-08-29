/*
 * SPDX-FileCopyrightText: 2010 Thomas McGuire <mcguire@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#pragma once

#include "messagecomposer_export.h"

#include <KJob>
#include <QUrl>

class QTextEdit;

namespace KIO
{
class Job;
}

namespace MessageComposer
{
class InsertTextFileJobPrivate;
/**
 * A job that downloads a given URL, interprets the result as a text file with the
 * given encoding and then inserts the text into the editor.
 */
class MESSAGECOMPOSER_EXPORT InsertTextFileJob : public KJob
{
    Q_OBJECT

public:
    explicit InsertTextFileJob(QTextEdit *editor, const QUrl &url);
    ~InsertTextFileJob() override;

    void setEncoding(const QString &encoding);
    void start() override;

private:
    void slotGetJobFinished(KJob *job);
    void slotFileData(KIO::Job *job, const QByteArray &data);
    std::unique_ptr<InsertTextFileJobPrivate> const d;
};
}

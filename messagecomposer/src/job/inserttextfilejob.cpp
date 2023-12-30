/*
 * SPDX-FileCopyrightText: 2010 Thomas McGuire <mcguire@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */
#include "inserttextfilejob.h"

#include <QTextEdit>

#include "messagecomposer_debug.h"
#include <KIO/TransferJob>

#include <QPointer>
#include <QStringDecoder>

using namespace MessageComposer;
class MessageComposer::InsertTextFileJobPrivate
{
public:
    InsertTextFileJobPrivate(QTextEdit *editor, const QUrl &url)
        : mEditor(editor)
        , mUrl(url)
    {
    }

    QPointer<QTextEdit> mEditor;
    QUrl mUrl;
    QString mEncoding;
    QByteArray mFileData;
};

InsertTextFileJob::InsertTextFileJob(QTextEdit *editor, const QUrl &url)
    : KJob(editor)
    , d(new MessageComposer::InsertTextFileJobPrivate(editor, url))
{
}

InsertTextFileJob::~InsertTextFileJob() = default;

void InsertTextFileJob::slotFileData(KIO::Job *job, const QByteArray &data)
{
    Q_UNUSED(job)
    d->mFileData += data;
}

void InsertTextFileJob::slotGetJobFinished(KJob *job)
{
    if (job->error()) {
        qCWarning(MESSAGECOMPOSER_LOG) << job->errorString();
        setError(job->error());
        setErrorText(job->errorText());
        emitResult();
        return;
    }

    if (d->mEditor) {
        if (!d->mEncoding.isEmpty()) {
            QStringDecoder fileCodec(d->mEncoding.toLatin1().constData());
            if (fileCodec.isValid()) {
                d->mEditor->textCursor().insertText(fileCodec.decode(d->mFileData.data()));
            } else {
                d->mEditor->textCursor().insertText(QString::fromLocal8Bit(d->mFileData.data()));
            }
        } else {
            d->mEditor->textCursor().insertText(QString::fromLocal8Bit(d->mFileData.data()));
        }
    }

    emitResult();
}

void InsertTextFileJob::setEncoding(const QString &encoding)
{
    d->mEncoding = encoding;
}

void InsertTextFileJob::start()
{
    KIO::TransferJob *job = KIO::get(d->mUrl);
    connect(job, &KIO::TransferJob::result, this, &InsertTextFileJob::slotGetJobFinished);
    connect(job, &KIO::TransferJob::data, this, &InsertTextFileJob::slotFileData);
    job->start();
}

#include "moc_inserttextfilejob.cpp"

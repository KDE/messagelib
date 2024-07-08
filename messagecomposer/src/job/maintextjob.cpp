/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "job/maintextjob.h"

#include "contentjobbase_p.h"
#include "job/multipartjob.h"
#include "job/singlepartjob.h"
#include "part/globalpart.h"
#include "part/textpart.h"
#include "utils/util.h"

#include <QStringEncoder>

#include "messagecomposer_debug.h"
#include <KLocalizedString>

#include <KMime/Content>

using namespace MessageComposer;

class MessageComposer::MainTextJobPrivate : public ContentJobBasePrivate
{
public:
    MainTextJobPrivate(MainTextJob *qq)
        : ContentJobBasePrivate(qq)
    {
    }

    [[nodiscard]] bool chooseSourcePlainText();
    [[nodiscard]] bool chooseCharsetAndEncode();
    [[nodiscard]] bool chooseCharset();
    [[nodiscard]] bool encodeTexts();
    [[nodiscard]] SinglepartJob *createPlainTextJob();
    [[nodiscard]] SinglepartJob *createHtmlJob();
    [[nodiscard]] SinglepartJob *createImageJob(const QSharedPointer<KPIMTextEdit::EmbeddedImage> &image);

    TextPart *textPart = nullptr;
    QByteArray chosenCharset;
    QString sourcePlainText;
    QByteArray encodedPlainText;
    QByteArray encodedHtml;

    Q_DECLARE_PUBLIC(MainTextJob)
};

bool MainTextJobPrivate::chooseSourcePlainText()
{
    Q_Q(MainTextJob);
    Q_ASSERT(textPart);
    if (textPart->isWordWrappingEnabled()) {
        sourcePlainText = textPart->wrappedPlainText();
        if (sourcePlainText.isEmpty() && !textPart->cleanPlainText().isEmpty()) {
            q->setError(JobBase::BugError);
            q->setErrorText(i18n("Asked to use word wrapping, but not given wrapped plain text."));
            return false;
        }
    } else {
        sourcePlainText = textPart->cleanPlainText();
        if (sourcePlainText.isEmpty() && !textPart->wrappedPlainText().isEmpty()) {
            q->setError(JobBase::BugError);
            q->setErrorText(i18n("Asked not to use word wrapping, but not given clean plain text."));
            return false;
        }
    }
    return true;
}

bool MainTextJobPrivate::chooseCharsetAndEncode()
{
    Q_Q(MainTextJob);

    Q_ASSERT(textPart);
    QString toTry = sourcePlainText;
    if (textPart->isHtmlUsed()) {
        toTry = textPart->cleanHtml();
    }
    chosenCharset = "utf-8";
    return encodeTexts();
}

bool MainTextJobPrivate::encodeTexts()
{
    Q_Q(MainTextJob);
    QStringEncoder codec(chosenCharset.constData());
    if (!codec.isValid()) {
        qCCritical(MESSAGECOMPOSER_LOG) << "Could not get text codec for charset" << chosenCharset;
        q->setError(JobBase::BugError);
        q->setErrorText(i18n("Could not get text codec for charset \"%1\".", QString::fromLatin1(chosenCharset)));
        return false;
    }
    encodedPlainText = codec.encode(sourcePlainText);
    if (!textPart->cleanHtml().isEmpty()) {
        encodedHtml = codec.encode(textPart->cleanHtml());
    }
    qCDebug(MESSAGECOMPOSER_LOG) << "Done.";
    return true;
}

SinglepartJob *MainTextJobPrivate::createPlainTextJob()
{
    auto cjob = new SinglepartJob; // No parent.
    cjob->contentType()->setMimeType("text/plain");
    cjob->contentType()->setCharset(chosenCharset);
    cjob->setData(encodedPlainText);
    // TODO standard recommends Content-ID.
    return cjob;
}

SinglepartJob *MainTextJobPrivate::createHtmlJob()
{
    auto cjob = new SinglepartJob; // No parent.
    cjob->contentType()->setMimeType("text/html");
    cjob->contentType()->setCharset(chosenCharset);
    const QByteArray data = KPIMTextEdit::RichTextComposerImages::imageNamesToContentIds(encodedHtml, textPart->embeddedImages());
    cjob->setData(data);
    // TODO standard recommends Content-ID.
    return cjob;
}

SinglepartJob *MainTextJobPrivate::createImageJob(const QSharedPointer<KPIMTextEdit::EmbeddedImage> &image)
{
    Q_Q(MainTextJob);

    // The image is a PNG encoded with base64.
    auto cjob = new SinglepartJob; // No parent.
    cjob->contentType()->setMimeType("image/png");
    cjob->contentType()->setName(image->imageName);
    cjob->contentTransferEncoding()->setEncoding(KMime::Headers::CEbase64);
    cjob->setDataIsEncoded(true); // It is already encoded.
    cjob->contentID()->setIdentifier(image->contentID.toLatin1());
    qCDebug(MESSAGECOMPOSER_LOG) << "cid" << cjob->contentID()->identifier();
    cjob->setData(image->image);
    return cjob;
}

MainTextJob::MainTextJob(TextPart *textPart, QObject *parent)
    : ContentJobBase(*new MainTextJobPrivate(this), parent)
{
    Q_D(MainTextJob);
    d->textPart = textPart;
}

MainTextJob::~MainTextJob() = default;

TextPart *MainTextJob::textPart() const
{
    Q_D(const MainTextJob);
    return d->textPart;
}

void MainTextJob::setTextPart(TextPart *part)
{
    Q_D(MainTextJob);
    d->textPart = part;
}

void MainTextJob::doStart()
{
    Q_D(MainTextJob);
    Q_ASSERT(d->textPart);

    // Word wrapping.
    if (!d->chooseSourcePlainText()) {
        // chooseSourcePlainText has set an error.
        Q_ASSERT(error());
        emitResult();
        return;
    }

    // Charset.
    if (!d->chooseCharsetAndEncode()) {
        // chooseCharsetAndEncode has set an error.
        Q_ASSERT(error());
        emitResult();
        return;
    }

    // Assemble the Content.
    SinglepartJob *plainJob = d->createPlainTextJob();
    if (d->encodedHtml.isEmpty()) {
        qCDebug(MESSAGECOMPOSER_LOG) << "Making text/plain";
        // Content is text/plain.
        appendSubjob(plainJob);
    } else {
        auto alternativeJob = new MultipartJob;
        alternativeJob->setMultipartSubtype("alternative");
        alternativeJob->appendSubjob(plainJob); // text/plain first.
        alternativeJob->appendSubjob(d->createHtmlJob()); // text/html second.
        if (!d->textPart->hasEmbeddedImages()) {
            qCDebug(MESSAGECOMPOSER_LOG) << "Have no images.  Making multipart/alternative.";
            // Content is multipart/alternative.
            appendSubjob(alternativeJob);
        } else {
            qCDebug(MESSAGECOMPOSER_LOG) << "Have related images.  Making multipart/related.";
            // Content is multipart/related with a multipart/alternative sub-Content.
            auto multipartJob = new MultipartJob;
            multipartJob->setMultipartSubtype("related");
            multipartJob->appendSubjob(alternativeJob);
            const auto embeddedImages = d->textPart->embeddedImages();
            for (const QSharedPointer<KPIMTextEdit::EmbeddedImage> &image : embeddedImages) {
                multipartJob->appendSubjob(d->createImageJob(image));
            }
            appendSubjob(multipartJob);
        }
    }
    ContentJobBase::doStart();
}

void MainTextJob::process()
{
    Q_D(MainTextJob);
    // The content has been created by our subjob.
    Q_ASSERT(d->subjobContents.count() == 1);
    d->resultContent = d->subjobContents.constFirst();
    emitResult();
}

#include "moc_maintextjob.cpp"

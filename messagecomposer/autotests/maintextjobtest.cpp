/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "maintextjobtest.h"
using namespace Qt::Literals::StringLiterals;

#include <QDebug>
#include <QStandardPaths>
#include <QStringDecoder>
#include <QTest>

#include <KMime/Content>

#include <KPIMTextEdit/RichTextComposerControler>
#include <KPIMTextEdit/RichTextComposerImages>
#include <MessageComposer/ComposerJob>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/MainTextJob>
#include <MessageComposer/RichTextComposerNg>
#include <MessageComposer/TextPart>

#include <KActionCollection>

using namespace MessageComposer;
using namespace KMime;

QTEST_MAIN(MainTextJobTest)

void MainTextJobTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
}

void MainTextJobTest::testPlainText()
{
    ComposerJob composerJob;
    composerJob.globalPart()->setGuiEnabled(false);
    auto textPart = new TextPart;
    QString data = u"they said their never they slept their dream"_s;
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart, &composerJob);
    QVERIFY(mjob->exec());
    Content *result = mjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), QByteArray("text/plain"));
    QCOMPARE(result->contentType()->charset(), QByteArray("utf-8"));
    QCOMPARE(QString::fromLatin1(result->body()), data);
    delete textPart;
}

void MainTextJobTest::testWrappingErrors()
{
    {
        ComposerJob composerJob;
        composerJob.globalPart()->setGuiEnabled(false);
        auto textPart = new TextPart;
        QString data = u"they said their never they slept their dream"_s;
        textPart->setWordWrappingEnabled(false);
        textPart->setWrappedPlainText(data);
        auto mjob = new MainTextJob(textPart, &composerJob);
        QVERIFY(!mjob->exec()); // error: not UseWrapping but given only wrapped text
        QCOMPARE(mjob->error(), int(JobBase::BugError));
        delete textPart;
    }
    {
        ComposerJob composerJob;
        composerJob.globalPart()->setGuiEnabled(false);
        auto textPart = new TextPart;
        textPart->setWordWrappingEnabled(true);
        QString data = u"they said their never they slept their dream"_s;
        textPart->setCleanPlainText(data);
        auto mjob = new MainTextJob(textPart, &composerJob);
        QVERIFY(!mjob->exec()); // error: UseWrapping but given only clean text
        QCOMPARE(mjob->error(), int(JobBase::BugError));
        delete textPart;
    }
}

void MainTextJobTest::testCustomCharset()
{
    ComposerJob composerJob;
    composerJob.globalPart()->setGuiEnabled(false);
    auto textPart = new TextPart;
    QString data = u"şi el o să se-nchidă cu o frunză de pelin"_s;
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart, &composerJob);
    QVERIFY(mjob->exec());
    Content *result = mjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), QByteArray("text/plain"));
    QCOMPARE(result->contentType()->charset(), "utf-8");
    QByteArray outData = result->body();
    QStringDecoder dec(QStringDecoder::Utf8);
    QVERIFY(dec.isValid());
    QCOMPARE(dec.decode(outData), data);

    delete textPart;
}

void MainTextJobTest::testNoCharset()
{
    ComposerJob composerJob;
    composerJob.globalPart()->setGuiEnabled(false);
    auto textPart = new TextPart;
    QString data = u"do you still play the accordion?"_s;
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart, &composerJob);
    QSKIP("This tests has been failing for a long time, please someone fix it", SkipSingle);
    QVERIFY(!mjob->exec()); // Error.
    QCOMPARE(mjob->error(), int(JobBase::BugError));
    qDebug() << mjob->errorString();
    delete textPart;
}

void MainTextJobTest::testBadCharset()
{
    ComposerJob composerJob;
    composerJob.globalPart()->setGuiEnabled(false);
    auto textPart = new TextPart;
    QString data = u"el a plâns peste ţară cu lacrima limbii noastre"_s;
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart, &composerJob);
    QSKIP("This tests has been failing for a long time, please someone fix it", SkipSingle);
    QVERIFY(!mjob->exec()); // Error.
    QCOMPARE(mjob->error(), int(JobBase::UserError));
    qDebug() << mjob->errorString();
    delete textPart;
}

void MainTextJobTest::testFallbackCharset()
{
    ComposerJob composerJob;
    composerJob.globalPart()->setGuiEnabled(false);
    auto textPart = new TextPart;
    QString data = u"and when he falleth..."_s;
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart, &composerJob);
    QVERIFY(mjob->exec());
    Content *result = mjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), QByteArray("text/plain"));
    QCOMPARE(result->contentType()->charset(), QByteArray("utf-8"));
    QCOMPARE(QString::fromLatin1(result->body()), data);
    delete textPart;
}

void MainTextJobTest::testHtml()
{
    QLatin1StringView originalHtml("<html><head></head><body>Test <em>with</em> formatting...<br>The end.</body></html>");
    MessageComposer::RichTextComposerNg editor;
    editor.createActions(new KActionCollection(this));
    editor.setTextOrHtml(originalHtml);
    QVERIFY(editor.composerControler()->isFormattingUsed());

    auto composerJob = new ComposerJob;
    composerJob->globalPart()->setGuiEnabled(false);
    auto textPart = new TextPart;
    textPart->setWordWrappingEnabled(false);
    textPart->setCleanPlainText(editor.composerControler()->toCleanPlainText());
    textPart->setCleanHtml(editor.toCleanHtml());
    auto mjob = new MainTextJob(textPart, composerJob);
    QVERIFY(mjob->exec());
    Content *result = mjob->content();
    result->assemble();
    qDebug() << result->encodedContent();

    // multipart/alternative
    {
        QVERIFY(result->contentType(false));
        QCOMPARE(result->contentType()->mimeType(), QByteArray("multipart/alternative"));
        QCOMPARE(result->contents().count(), 2);
        // text/plain
        {
            Content *plain = result->contents().at(0);
            QVERIFY(plain->contentType(false));
            QCOMPARE(plain->contentType()->mimeType(), QByteArray("text/plain"));
            QCOMPARE(QString::fromLatin1(plain->body()), editor.composerControler()->toCleanPlainText());
        }
        // text/html
        {
            Content *html = result->contents().at(1);
            QVERIFY(html->contentType(false));
            QCOMPARE(html->contentType()->mimeType(), QByteArray("text/html"));
            // The editor adds extra Html stuff, so we can't compare to originalHtml.
            QCOMPARE(QLatin1StringView(html->body()), editor.toCleanHtml());
        }
    }
    delete composerJob;
}

void MainTextJobTest::testHtmlWithImages()
{
    KActionCollection ac(this);
    MessageComposer::RichTextComposerNg editor;
    editor.createActions(new KActionCollection(this));

    QImage image1(16, 16, QImage::Format_ARGB32_Premultiplied);
    image1.fill(Qt::red);
    const QString image1Path = QCoreApplication::applicationDirPath() + QLatin1StringView("/image1.png");
    image1.save(image1Path);

    QImage image2(16, 16, QImage::Format_ARGB32_Premultiplied);
    image2.fill(Qt::blue);
    const QString image2Path = QCoreApplication::applicationDirPath() + QLatin1StringView("/image2.png");
    image2.save(image2Path);

    QString data = u"dust in the wind"_s;
    editor.setTextOrHtml(data);
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image1Path));
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image1Path));
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image2Path));
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image2Path));

    auto composerJob = new ComposerJob;
    composerJob->globalPart()->setGuiEnabled(false);
    auto textPart = new TextPart;
    textPart->setWordWrappingEnabled(false);
    textPart->setCleanPlainText(editor.composerControler()->toCleanPlainText());
    textPart->setCleanHtml(editor.composerControler()->toCleanHtml());
    // only get once, are regenerated on call with new contentID each time
    const KPIMTextEdit::ImageList images = editor.composerControler()->composerImages()->embeddedImages();
    QCOMPARE(images.count(), 2);
    const QString cid1 = images[0]->contentID;
    const QString cid2 = images[1]->contentID;
    const QString name1 = images[0]->imageName;
    const QString name2 = images[1]->imageName;
    textPart->setEmbeddedImages(images);
    auto mjob = new MainTextJob(textPart, composerJob);
    QVERIFY(mjob->exec());
    Content *result = mjob->content();
    result->assemble();
    qDebug() << result->encodedContent();

    // multipart/related
    {
        QVERIFY(result->contentType(false));
        QCOMPARE(result->contentType()->mimeType(), QByteArray("multipart/related"));
        QCOMPARE(result->contents().count(), 3);
        // multipart/alternative
        {
            Content *alternative = result->contents().at(0);
            QVERIFY(alternative->contentType(false));
            QCOMPARE(alternative->contentType()->mimeType(), QByteArray("multipart/alternative"));
            QCOMPARE(alternative->contents().count(), 2);
            // text/plain
            {
                Content *plain = alternative->contents().at(0);
                QCOMPARE(plain->contentType()->mimeType(), QByteArray("text/plain"));
                QCOMPARE(QString::fromLatin1(plain->body()), data);
            }
            // text/html
            {
                Content *html = alternative->contents().at(1);
                QCOMPARE(html->contentType()->mimeType(), QByteArray("text/html"));
                QString data = QString::fromLatin1(html->body());
                const int idx1 = data.indexOf(u"cid:%1"_s.arg(cid1));
                const int idx2 = data.indexOf(u"cid:%1"_s.arg(cid2));
                // qDebug() << " cid1 " << cid1 << "cid2 " << cid2 << " data " << data;
                QVERIFY(idx1 > 0);
                QVERIFY(idx2 > 0);
                QVERIFY(idx1 < idx2);
            }
        }
        // First image/png
        {
            const Content *image = result->contents().at(1);
            QVERIFY(image->contentType());
            QCOMPARE(image->contentType()->mimeType(), QByteArray("image/png"));
            QCOMPARE(image->contentType()->name(), name1);
            const Headers::ContentID *cid = image->contentID();
            QVERIFY(cid);
            QCOMPARE(cid->identifier(), cid1.toLatin1());
        }
        // Second image/png
        {
            const Content *image = result->contents().at(2);
            QVERIFY(image->contentType());
            QCOMPARE(image->contentType()->mimeType(), QByteArray("image/png"));
            QCOMPARE(image->contentType()->name(), name2);
            const Headers::ContentID *cid = image->contentID();
            QVERIFY(cid);
            QCOMPARE(cid->identifier(), cid2.toLatin1());
        }
    }
    delete textPart;
    delete composerJob;
}

#include "moc_maintextjobtest.cpp"

/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "maintextjobtest.h"

#include <QStandardPaths>
#include <QTextCodec>

#include <QDebug>
#include <QTest>

#include <kmime/kmime_content.h>
using namespace KMime;

#include <KPIMTextEdit/RichTextComposerControler>
#include <KPIMTextEdit/RichTextComposerImages>
#include <MessageComposer/Composer>
#include <MessageComposer/GlobalPart>
#include <MessageComposer/MainTextJob>
#include <MessageComposer/RichTextComposerNg>
#include <MessageComposer/TextPart>

#include <KActionCollection>

//#include <kpimtextedit/textedit.h>

using namespace MessageComposer;

QTEST_MAIN(MainTextJobTest)

void MainTextJobTest::initTestCase()
{
    qputenv("KDE_FORK_SLAVES", "yes"); // To avoid a runtime dependency on klauncher
    QStandardPaths::setTestModeEnabled(true);
}

void MainTextJobTest::testPlainText()
{
    auto composer = Composer();
    composer.globalPart()->setGuiEnabled(false);
    QVector<QByteArray> charsets;
    charsets << "us-ascii"
             << "utf-8";
    composer.globalPart()->setCharsets(charsets);
    auto textPart = new TextPart;
    QString data = QStringLiteral("they said their nevers they slept their dream");
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart, &composer);
    QVERIFY(mjob->exec());
    Content *result = mjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), QByteArray("text/plain"));
    QCOMPARE(result->contentType()->charset(), QByteArray("us-ascii"));
    QCOMPARE(QString::fromLatin1(result->body()), data);
    delete textPart;
}

void MainTextJobTest::testWrappingErrors()
{
    {
        auto composer = Composer();
        composer.globalPart()->setGuiEnabled(false);
        composer.globalPart()->setFallbackCharsetEnabled(true);
        auto textPart = new TextPart;
        QString data = QStringLiteral("they said their nevers they slept their dream");
        textPart->setWordWrappingEnabled(false);
        textPart->setWrappedPlainText(data);
        auto mjob = new MainTextJob(textPart, &composer);
        QVERIFY(!mjob->exec()); // error: not UseWrapping but given only wrapped text
        QCOMPARE(mjob->error(), int(JobBase::BugError));
        delete textPart;
    }
    {
        auto composer = Composer();
        composer.globalPart()->setGuiEnabled(false);
        composer.globalPart()->setFallbackCharsetEnabled(true);
        auto textPart = new TextPart;
        textPart->setWordWrappingEnabled(true);
        QString data = QStringLiteral("they said their nevers they slept their dream");
        textPart->setCleanPlainText(data);
        auto mjob = new MainTextJob(textPart, &composer);
        QVERIFY(!mjob->exec()); // error: UseWrapping but given only clean text
        QCOMPARE(mjob->error(), int(JobBase::BugError));
        delete textPart;
    }
}

void MainTextJobTest::testCustomCharset()
{
    auto composer = Composer();
    composer.globalPart()->setGuiEnabled(false);
    QByteArray charset("iso-8859-2");
    composer.globalPart()->setCharsets(QVector<QByteArray>() << charset);
    auto textPart = new TextPart;
    QString data = QStringLiteral("şi el o să se-nchidă cu o frunză de pelin");
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart, &composer);
    QVERIFY(mjob->exec());
    Content *result = mjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), QByteArray("text/plain"));
    QCOMPARE(result->contentType()->charset(), charset);
    QByteArray outData = result->body();
    QTextCodec *codec = QTextCodec::codecForName(charset);
    QVERIFY(codec);
    QCOMPARE(codec->toUnicode(outData), data);
    delete textPart;
}

void MainTextJobTest::testNoCharset()
{
    auto composer = Composer();
    QVERIFY(!composer.globalPart()->isFallbackCharsetEnabled());
    composer.globalPart()->setGuiEnabled(false);
    auto textPart = new TextPart;
    QString data = QStringLiteral("do you still play the accordion?");
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart, &composer);
    QSKIP("This tests has been failing for a long time, please someone fix it", SkipSingle);
    QVERIFY(!mjob->exec()); // Error.
    QCOMPARE(mjob->error(), int(JobBase::BugError));
    qDebug() << mjob->errorString();
    delete textPart;
}

void MainTextJobTest::testBadCharset()
{
    auto composer = Composer();
    composer.globalPart()->setGuiEnabled(false);
    QByteArray charset("us-ascii"); // Cannot handle Romanian chars.
    composer.globalPart()->setCharsets(QVector<QByteArray>() << charset);
    auto textPart = new TextPart;
    QString data = QStringLiteral("el a plâns peste ţară cu lacrima limbii noastre");
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart, &composer);
    QSKIP("This tests has been failing for a long time, please someone fix it", SkipSingle);
    QVERIFY(!mjob->exec()); // Error.
    QCOMPARE(mjob->error(), int(JobBase::UserError));
    qDebug() << mjob->errorString();
    delete textPart;
}

void MainTextJobTest::testFallbackCharset()
{
    auto composer = Composer();
    composer.globalPart()->setGuiEnabled(false);
    composer.globalPart()->setFallbackCharsetEnabled(true);
    auto textPart = new TextPart;
    QString data = QStringLiteral("and when he falleth...");
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart, &composer);
    QVERIFY(mjob->exec());
    Content *result = mjob->content();
    result->assemble();
    qDebug() << result->encodedContent();
    QVERIFY(result->contentType(false));
    QCOMPARE(result->contentType()->mimeType(), QByteArray("text/plain"));
    QCOMPARE(result->contentType()->charset(), QByteArray("us-ascii")); // Fallback is us-ascii or utf8.
    QCOMPARE(QString::fromLatin1(result->body()), data);
    delete textPart;
}

void MainTextJobTest::testHtml()
{
    QLatin1String originalHtml("<html><head></head><body>Test <em>with</em> formatting...<br>The end.</body></html>");
    MessageComposer::RichTextComposerNg editor;
    editor.createActions(new KActionCollection(this));
    editor.setTextOrHtml(originalHtml);
    QVERIFY(editor.composerControler()->isFormattingUsed());

    auto composer = new Composer;
    composer->globalPart()->setGuiEnabled(false);
    composer->globalPart()->setFallbackCharsetEnabled(true);
    auto textPart = new TextPart;
    textPart->setWordWrappingEnabled(false);
    textPart->setCleanPlainText(editor.composerControler()->toCleanPlainText());
    textPart->setCleanHtml(editor.toCleanHtml());
    auto mjob = new MainTextJob(textPart, composer);
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
            QCOMPARE(QLatin1String(html->body()), editor.toCleanHtml());
        }
    }
    delete composer;
}

void MainTextJobTest::testHtmlWithImages()
{
    KActionCollection ac(this);
    MessageComposer::RichTextComposerNg editor;
    editor.createActions(new KActionCollection(this));

    QImage image1(16, 16, QImage::Format_ARGB32_Premultiplied);
    image1.fill(Qt::red);
    const QString image1Path = QCoreApplication::applicationDirPath() + QLatin1String("/image1.png");
    image1.save(image1Path);

    QImage image2(16, 16, QImage::Format_ARGB32_Premultiplied);
    image2.fill(Qt::blue);
    const QString image2Path = QCoreApplication::applicationDirPath() + QLatin1String("/image2.png");
    image2.save(image2Path);

    QString data = QStringLiteral("dust in the wind");
    editor.setTextOrHtml(data);
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image1Path));
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image1Path));
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image2Path));
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image2Path));

    auto composer = new Composer;
    composer->globalPart()->setGuiEnabled(false);
    composer->globalPart()->setFallbackCharsetEnabled(true);
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
    auto mjob = new MainTextJob(textPart, composer);
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
                int idx1 = data.indexOf(QStringLiteral("cid:%1").arg(cid1));
                int idx2 = data.indexOf(QStringLiteral("cid:%1").arg(cid2));
                QVERIFY(idx1 > 0);
                QVERIFY(idx2 > 0);
                QVERIFY(idx1 < idx2);
            }
        }
        // First image/png
        {
            Content *image = result->contents().at(1);
            QVERIFY(image->contentType(false));
            QCOMPARE(image->contentType()->mimeType(), QByteArray("image/png"));
            QCOMPARE(image->contentType()->name(), name1);
            const Headers::ContentID *cid = image->header<Headers::ContentID>();
            QVERIFY(cid);
            QCOMPARE(cid->identifier(), cid1.toLatin1());
        }
        // Second image/png
        {
            Content *image = result->contents().at(2);
            QVERIFY(image->contentType(false));
            QCOMPARE(image->contentType()->mimeType(), QByteArray("image/png"));
            QCOMPARE(image->contentType()->name(), name2);
            const Headers::ContentID *cid = image->header<Headers::ContentID>();
            QVERIFY(cid);
            QCOMPARE(cid->identifier(), cid2.toLatin1());
        }
    }
    delete textPart;
    delete composer;
}

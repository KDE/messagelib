/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "maintextjobtest.h"

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

using namespace Qt::Literals::StringLiterals;
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
    auto textPart = std::make_unique<TextPart>();
    const QString data = u"they said their never they slept their dream"_s;
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart.get(), &composerJob);
    QVERIFY(mjob->exec());
    const auto result = mjob->takeContent();
    result->assemble();
    qDebug() << result->encodedContent();
    QVERIFY(result->contentType(KMime::CreatePolicy::DontCreate));
    QCOMPARE(result->contentType()->mimeType(), QByteArray("text/plain"));
    QCOMPARE(result->contentType()->charset(), QByteArray("utf-8"));
    QCOMPARE(QString::fromLatin1(result->body()), data);
}

void MainTextJobTest::testWrappingErrors()
{
    {
        ComposerJob composerJob;
        composerJob.globalPart()->setGuiEnabled(false);
        auto textPart = std::make_unique<TextPart>();
        const QString data = u"they said their never they slept their dream"_s;
        textPart->setWordWrappingEnabled(false);
        textPart->setWrappedPlainText(data);
        auto mjob = new MainTextJob(textPart.get(), &composerJob);
        QVERIFY(!mjob->exec()); // error: not UseWrapping but given only wrapped text
        QCOMPARE(mjob->error(), int(JobBase::BugError));
    }
    {
        ComposerJob composerJob;
        composerJob.globalPart()->setGuiEnabled(false);
        auto textPart = std::make_unique<TextPart>();
        textPart->setWordWrappingEnabled(true);
        const QString data = u"they said their never they slept their dream"_s;
        textPart->setCleanPlainText(data);
        auto mjob = new MainTextJob(textPart.get(), &composerJob);
        QVERIFY(!mjob->exec()); // error: UseWrapping but given only clean text
        QCOMPARE(mjob->error(), int(JobBase::BugError));
    }
}

void MainTextJobTest::testCustomCharset()
{
    ComposerJob composerJob;
    composerJob.globalPart()->setGuiEnabled(false);
    auto textPart = std::make_unique<TextPart>();
    const QString data = u"şi el o să se-nchidă cu o frunză de pelin"_s;
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart.get(), &composerJob);
    QVERIFY(mjob->exec());
    const auto result = mjob->takeContent();
    result->assemble();
    qDebug() << result->encodedContent();
    QVERIFY(result->contentType(KMime::CreatePolicy::DontCreate));
    QCOMPARE(result->contentType()->mimeType(), QByteArray("text/plain"));
    QCOMPARE(result->contentType()->charset(), "utf-8");
    const QByteArray outData = result->body();
    QStringDecoder dec(QStringDecoder::Utf8);
    QVERIFY(dec.isValid());
    QCOMPARE(dec.decode(outData), data);
}

void MainTextJobTest::testNoCharset()
{
    ComposerJob composerJob;
    composerJob.globalPart()->setGuiEnabled(false);
    auto textPart = std::make_unique<TextPart>();
    const QString data = u"do you still play the accordion?"_s;
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart.get(), &composerJob);
    QSKIP("This tests has been failing for a long time, please someone fix it", SkipSingle);
    QVERIFY(!mjob->exec()); // Error.
    QCOMPARE(mjob->error(), int(JobBase::BugError));
    qDebug() << mjob->errorString();
}

void MainTextJobTest::testBadCharset()
{
    ComposerJob composerJob;
    composerJob.globalPart()->setGuiEnabled(false);
    auto textPart = std::make_unique<TextPart>();
    const QString data = u"el a plâns peste ţară cu lacrima limbii noastre"_s;
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart.get(), &composerJob);
    QSKIP("This tests has been failing for a long time, please someone fix it", SkipSingle);
    QVERIFY(!mjob->exec()); // Error.
    QCOMPARE(mjob->error(), int(JobBase::UserError));
    qDebug() << mjob->errorString();
}

void MainTextJobTest::testFallbackCharset()
{
    ComposerJob composerJob;
    composerJob.globalPart()->setGuiEnabled(false);
    auto textPart = std::make_unique<TextPart>();
    const QString data = u"and when he falleth..."_s;
    textPart->setWrappedPlainText(data);
    auto mjob = new MainTextJob(textPart.get(), &composerJob);
    QVERIFY(mjob->exec());
    const auto result = mjob->takeContent();
    result->assemble();
    qDebug() << result->encodedContent();
    QVERIFY(result->contentType(KMime::CreatePolicy::DontCreate));
    QCOMPARE(result->contentType()->mimeType(), QByteArray("text/plain"));
    QCOMPARE(result->contentType()->charset(), QByteArray("utf-8"));
    QCOMPARE(QString::fromLatin1(result->body()), data);
}

void MainTextJobTest::testHtml()
{
    const QLatin1StringView originalHtml("<html><head></head><body>Test <em>with</em> formatting...<br>The end.</body></html>");
    MessageComposer::RichTextComposerNg editor;
    editor.createActions(new KActionCollection(this));
    editor.setTextOrHtml(originalHtml);
    QVERIFY(editor.composerControler()->isFormattingUsed());

    auto composerJob = new ComposerJob;
    composerJob->globalPart()->setGuiEnabled(false);
    auto textPart = std::make_unique<TextPart>();
    textPart->setWordWrappingEnabled(false);
    textPart->setCleanPlainText(editor.composerControler()->toCleanPlainText());
    textPart->setCleanHtml(editor.toCleanHtml());
    auto mjob = new MainTextJob(textPart.get(), composerJob);
    QVERIFY(mjob->exec());
    const auto result = mjob->takeContent();
    result->assemble();
    qDebug() << result->encodedContent();

    // multipart/alternative
    {
        QVERIFY(result->contentType(KMime::CreatePolicy::DontCreate));
        QCOMPARE(result->contentType()->mimeType(), QByteArray("multipart/alternative"));
        QCOMPARE(result->contents().count(), 2);
        // text/plain
        {
            Content *plain = result->contents().at(0);
            QVERIFY(plain->contentType(KMime::CreatePolicy::DontCreate));
            QCOMPARE(plain->contentType()->mimeType(), QByteArray("text/plain"));
            QCOMPARE(QString::fromLatin1(plain->body()), editor.composerControler()->toCleanPlainText());
        }
        // text/html
        {
            Content *html = result->contents().at(1);
            QVERIFY(html->contentType(KMime::CreatePolicy::DontCreate));
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
    const QString image1Path = QCoreApplication::applicationDirPath() + "/image1.png"_L1;
    image1.save(image1Path);

    QImage image2(16, 16, QImage::Format_ARGB32_Premultiplied);
    image2.fill(Qt::blue);
    const QString image2Path = QCoreApplication::applicationDirPath() + "/image2.png"_L1;
    image2.save(image2Path);

    const QString data = u"dust in the wind"_s;
    editor.setTextOrHtml(data);
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image1Path));
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image1Path));
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image2Path));
    editor.composerControler()->composerImages()->addImage(QUrl::fromLocalFile(image2Path));

    auto composerJob = new ComposerJob;
    composerJob->globalPart()->setGuiEnabled(false);
    auto textPart = new TextPart;
    editor.fillComposerTextPart(textPart);
    textPart->setWordWrappingEnabled(false);
    QVERIFY(textPart->cleanPlainText().contains(data));
    const EmbeddedImageList images = textPart->embeddedImages();
    QCOMPARE(images.count(), 2);
    const QString cid1 = images[0].contentID;
    const QString cid2 = images[1].contentID;
    const QString name1 = images[0].imageName;
    const QString name2 = images[1].imageName;
    auto mjob = new MainTextJob(textPart, composerJob);
    QVERIFY(mjob->exec());
    const auto result = mjob->takeContent();
    result->assemble();
    qDebug() << result->encodedContent();

    // multipart/related
    {
        QVERIFY(result->contentType(KMime::CreatePolicy::DontCreate));
        QCOMPARE(result->contentType()->mimeType(), QByteArray("multipart/related"));
        QCOMPARE(result->contents().count(), 3);
        // multipart/alternative
        {
            Content *alternative = result->contents().at(0);
            QVERIFY(alternative->contentType(KMime::CreatePolicy::DontCreate));
            QCOMPARE(alternative->contentType()->mimeType(), QByteArray("multipart/alternative"));
            QCOMPARE(alternative->contents().count(), 2);
            // text/plain
            {
                Content *plain = alternative->contents().at(0);
                QCOMPARE(plain->contentType()->mimeType(), QByteArray("text/plain"));
                QCOMPARE(QString::fromLatin1(plain->body()), textPart->cleanPlainText());
            }
            // text/html
            {
                Content *html = alternative->contents().at(1);
                QCOMPARE(html->contentType()->mimeType(), QByteArray("text/html"));
                const QString body = QString::fromLatin1(html->body());
                const int idx1 = body.indexOf(u"cid:%1"_s.arg(cid1));
                const int idx2 = body.indexOf(u"cid:%1"_s.arg(cid2));
                // qDebug() << " cid1 " << cid1 << "cid2 " << cid2 << " body " << body;
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

void MainTextJobTest::testEmbeddedImageData_data()
{
    QTest::addColumn<QString>("imageName");
    QTest::newRow("ascii") << u"image.png"_s;
    QTest::newRow("unicode") << u"café-日本語.png"_s;
}

void MainTextJobTest::testEmbeddedImageData()
{
    QFETCH(QString, imageName);

    const QByteArray pngData = QByteArray::fromHex("89504e470d0a1a0a");
    TextPart textPart;
    textPart.setWordWrappingEnabled(false);
    textPart.setCleanPlainText(u"An embedded image"_s);
    textPart.setCleanHtml(u"<img src=\"%1\"><img src=\"prefix-%1\"><img src=\"cid:existing\">"_s.arg(imageName));
    textPart.setEmbeddedImages({{pngData.toBase64(), u"image@example.org"_s, imageName}});

    MainTextJob job(&textPart);
    QVERIFY(job.exec());
    const auto result = job.takeContent();
    result->assemble();
    QCOMPARE(result->contentType()->mimeType(), QByteArray("multipart/related"));
    QCOMPARE(result->contents().size(), 2);
    const auto alternative = result->contents().at(0);
    QCOMPARE(alternative->contentType()->mimeType(), QByteArray("multipart/alternative"));
    QCOMPARE(alternative->contents().size(), 2);
    const auto html = alternative->contents().at(1);
    QCOMPARE(html->decodedBody(), u"<img src=\"cid:image@example.org\"><img src=\"prefix-%1\"><img src=\"cid:existing\">"_s.arg(imageName).toUtf8());
    const auto image = result->contents().at(1);
    QCOMPARE(image->contentType()->mimeType(), QByteArray("image/png"));
    QCOMPARE(image->contentType()->name(), imageName);
    QCOMPARE(image->contentID()->identifier(), QByteArray("image@example.org"));
    QCOMPARE(image->contentTransferEncoding()->encoding(), KMime::Headers::CEbase64);
    QCOMPARE(image->decodedBody(), pngData);
}

#include "moc_maintextjobtest.cpp"

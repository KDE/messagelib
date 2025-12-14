/*
  SPDX-FileCopyrightText: 2010 Volker Krause <vkrause@kde.org>
  SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include <QProcess>

static QString renderTreeHelper(const MimeTreeParser::MessagePart::Ptr &messagePart, QString indent)
{
    const QString line = u"%1 * %3\n"_s.arg(indent, QString::fromUtf8(messagePart->metaObject()->className()));
    QString ret = line;

    indent += u' ';
    const auto subParts = messagePart->subParts();
    for (const auto &subPart : subParts) {
        ret += renderTreeHelper(subPart, indent);
    }
    return ret;
}

void RenderTest::testRenderTree(const MimeTreeParser::MessagePart::Ptr &messagePart)
{
    QString renderedTree = renderTreeHelper(messagePart, QString());

    QFETCH(QString, mailFileName);
    QFETCH(QString, outFileName);
    const QString treeFileName = QLatin1StringView(MAIL_DATA_DIR) + u'/' + mailFileName + u".tree"_s;
    const QString outTreeFileName = outFileName + u".tree"_s;

    {
        QFile f(outTreeFileName);
        f.open(QIODevice::WriteOnly);
        f.write(renderedTree.toUtf8());
        f.close();
    }
    // compare to reference file
    const QStringList args = QStringList() << u"-u"_s << treeFileName << outTreeFileName;
    QProcess proc;
    proc.setProcessChannelMode(QProcess::ForwardedChannels);
    proc.start(u"diff"_s, args);
    QVERIFY(proc.waitForFinished());

    QCOMPARE(proc.exitCode(), 0);
}

void RenderTest::testRender()
{
    QFETCH(QString, mailFileName);
    QFETCH(QString, referenceFileName);
    QFETCH(QString, outFileName);
    QFETCH(QString, attachmentStrategy);
    QFETCH(bool, showSignatureDetails);
    QFETCH(QString, asyncFileName);

    if (mailFileName == u"openpgp-encrypted-no-text-attachment-non-encrypted.mbox"_s || mailFileName == u"openpgp-encrypted-no-text-attachment.mbox"_s
        || mailFileName == u"smime-signed-apple.mbox"_s) {
        QSKIP("FIXME: Failing for too long");
    }

    // const QString htmlFileName = outFileName + u".html"_s;
    const bool bAsync = !asyncFileName.isEmpty();

    // load input mail
    std::shared_ptr<KMime::Message> msg(Test::readAndParseMail(mailFileName));

    // render the mail
    FileHtmlWriter fileWriter(outFileName);
    QImage paintDevice;
    Test::TestCSSHelper cssHelper(&paintDevice);
    MimeTreeParser::NodeHelper nodeHelper;
    Test::ObjectTreeSource testSource(&fileWriter, &cssHelper);
    testSource.setAllowDecryption(true);
    testSource.setAttachmentStrategy(attachmentStrategy);
    testSource.setShowSignatureDetails(showSignatureDetails);

    QEventLoop loop;
    MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
    connect(&nodeHelper, &MimeTreeParser::NodeHelper::update, &loop, &QEventLoop::quit);
    otp.setAllowAsync(bAsync);

    otp.parseObjectTree(msg.get());

    fileWriter.begin();
    CSSHelperBase::HtmlHeadSettings htmlHeadSettings;
    fileWriter.write(cssHelper.htmlHead(htmlHeadSettings));
    testSource.render(otp.parsedPart(), false);
    fileWriter.write(u"</body></html>"_s);
    fileWriter.end();

    if (!bAsync) {
        testRenderTree(otp.parsedPart());
    } else {
        Test::compareFile(outFileName, asyncFileName);
        loop.exec();

        MimeTreeParser::ObjectTreeParser otp(&testSource, &nodeHelper);
        otp.setAllowAsync(bAsync);

        otp.parseObjectTree(msg.get());

        fileWriter.begin();
        CSSHelperBase::HtmlHeadSettings htmlHeadSettings;
        fileWriter.write(cssHelper.htmlHead(htmlHeadSettings));
        testSource.render(otp.parsedPart(), false);
        fileWriter.write(u"</body></html>"_s);
        fileWriter.end();

        testRenderTree(otp.parsedPart());
    }
    Test::compareFile(outFileName, referenceFileName);
    msg.reset();
}

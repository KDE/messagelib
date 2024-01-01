/*
  SPDX-FileCopyrightText: 2015-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "converthtmltoplaintexttest.h"
#include "../converthtmltoplaintext.h"
#include <QTest>

ConvertHtmlToPlainTextTest::ConvertHtmlToPlainTextTest(QObject *parent)
    : QObject(parent)
{
}

ConvertHtmlToPlainTextTest::~ConvertHtmlToPlainTextTest() = default;

void ConvertHtmlToPlainTextTest::shouldHaveDefaultValue()
{
    MimeTreeParser::ConvertHtmlToPlainText convert;
    QVERIFY(convert.htmlString().isEmpty());
}

void ConvertHtmlToPlainTextTest::shouldReturnEmptyStringIfInputTextIsEmpty()
{
    MimeTreeParser::ConvertHtmlToPlainText convert;
    convert.setHtmlString(QString());
    QVERIFY(convert.generatePlainText().isEmpty());
}

void ConvertHtmlToPlainTextTest::shouldReturnNotEmptyStringIfInputTextIsNotEmpty()
{
    MimeTreeParser::ConvertHtmlToPlainText convert;
    const QString str = QStringLiteral("foo bla");
    convert.setHtmlString(str);
    const QString result = convert.generatePlainText();
    QVERIFY(!result.isEmpty());
    QCOMPARE(result, QString(str + QLatin1Char('\n')));
}

void ConvertHtmlToPlainTextTest::shouldConvertToPlainText_data()
{
    QTest::addColumn<QString>("inputText");
    QTest::addColumn<QString>("convertedText");
    QTest::newRow("plainText") << QStringLiteral("foo") << QStringLiteral("foo\n");
    QTest::newRow("htmlText") << QStringLiteral("<html><body>Hi! This is a KDE test</body></html>") << QStringLiteral("Hi! This is a KDE test\n");
    QTest::newRow("htmlTextWithBold") << QStringLiteral("<html><body><b>Hi!</b> This is a KDE test</body></html>")
                                      << QStringLiteral("*Hi!* This is a KDE test\n");
    QTest::newRow("htmlTextWithH1") << QStringLiteral("<html><body><h1>Hi!</h1> This is a KDE test</body></html>")
                                    << QStringLiteral("*Hi!*\nThis is a KDE test\n");
    QTest::newRow("htmlTextWithUnderLine") << QStringLiteral("<html><body><u>Hi!</u> This is a KDE test</body></html>")
                                           << QStringLiteral("_Hi!_ This is a KDE test\n");
    QTest::newRow("bug345360") << QStringLiteral(
        "<html><head><title>Changes to Zoho Privacy Policy and Terms of Service</title></head><body><center><table width=\"600\" style=\"margin-top:20px;\"  "
        "cellspacing=\"0\" cellpadding=\"0\" border=\"0\"><tr><td style=\"text-align:left;\"><div id=\"_zc_default\" style=\"font-family: "
        "Arial,Helvetica,sans-serif; font-size: 12px;padding-top:0px;padding-bottom:0px\"><span style=\"font-family: verdana , arial , helvetica , "
        "sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\">    Dear User,       </span> <br style=\"font-family: verdana , arial , "
        "helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\"> <br style=\"font-family: verdana , arial , helvetica , "
        "sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\"> <span style=\"font-family: verdana , arial , helvetica , "
        "sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\">    We     have updated our Privacy Policy and Terms of Service. Please take "
        "a    few  minutes to read and understand them. To help you understand, we    have  summarized the changes and provided a comparison of the current    "
        "and the  previous versions.       </span> <br style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: "
        "rgb(255,255,255);\"> <br style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\"> "
        "<span style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\">    These     "
        "changes will take effect on April 19, 2015.&nbsp; If you continue to use     Zoho after April 19, 2015, you will be governed by the new Privacy     "
        "Policy and Terms of Service.       </span> <br style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: "
        "rgb(255,255,255);\"> <br style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\"> "
        "<span style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\">    "
        "&nbsp;&nbsp;&nbsp;&nbsp;       </span> <a "
        "href=\"http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b3d\" target=\"_blank\">    Revised Privacy "
        "Policy       </a> <br style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\"> "
        "<span style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\">    "
        "&nbsp;&nbsp;&nbsp;&nbsp;       </span> <a "
        "href=\"http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b3f\" target=\"_blank\">    Summary of "
        "Changes to Privacy Policy       </a> <br style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: "
        "rgb(255,255,255);\"> <span style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: "
        "rgb(255,255,255);\">    &nbsp;&nbsp;&nbsp;&nbsp;       </span> <a "
        "href=\"http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b41\" target=\"_blank\">    Comparison      "
        " </a> <br style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\"> <br "
        "style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\"> <span "
        "style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\">    "
        "&nbsp;&nbsp;&nbsp;&nbsp;       </span> <a "
        "href=\"http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b47\" target=\"_blank\">    Revised Terms "
        "of Service       </a> <br style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\"> "
        "<span style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\">    "
        "&nbsp;&nbsp;&nbsp;&nbsp;       </span> <a "
        "href=\"http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b45\" target=\"_blank\">    Summary of "
        "Changes to Terms of Service       </a> <br style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: "
        "rgb(255,255,255);\"> <span style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: "
        "rgb(255,255,255);\">    &nbsp;&nbsp;&nbsp;&nbsp;       </span> <a "
        "href=\"http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b43\" target=\"_blank\">    Comparison      "
        " </a> <br> <br> <span style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\"> "
        "<br>Thanks!<br><br></span><div><span style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: "
        "rgb(255,255,255);\">The Zoho Team&nbsp; </span> <div>    <span style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: "
        "13.333333px;background-color: rgb(255,255,255);\">       Zoho Corporation</span> </div> <div>    <span style=\"font-family: verdana , arial , "
        "helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\">       4141 Hacienda Drive        Pleasanton,&nbsp;    </span> "
        "</div> <div>    <span style=\"font-family: verdana , arial , helvetica , sans-serif;font-size: 13.333333px;background-color: rgb(255,255,255);\">     "
        "  CA 94588,</span><span style=\"font-size: 13.333333px; font-family: verdana, arial, helvetica, sans-serif; background-color: rgb(255, 255, "
        "255);\">USA      </span><span style=\"font-size: 10pt;\">    </span><span "
        "data-href=\"http://zc1.maillist-manage.com/ua/optout?od=11287eca375dad&rd=189bb4a86db70a&sd=189bb4a7ed6b37&n=11699e4c206dee1\" data-target "
        "data-text-len=\"4\" data-type=\"1\" style=\"font-size: 10pt; color: rgb(255, 255, 255);\">       .</span></div></div></div><img "
        "src=\"http://zc1.maillist-manage.com/open/od11287eca375dad_rd189bb4a86db70a.gif\" height=\"1\" "
        "width=\"50\"/></td></tr></table></center></body></html>")
                               << QStringLiteral(
                                      "\nDear User, \n\nWe have updated our Privacy Policy and Terms of Service. Please take a few minutes to read and "
                                      "understand them. To help you understand, we have summarized the changes and provided a comparison of the current and "
                                      "the previous versions. \n\nThese changes will take effect on April 19, 2015.  If you continue to use Zoho after April "
                                      "19, 2015, you will be governed by the new Privacy Policy and Terms of Service. \n\n     Revised Privacy Policy [1]\n    "
                                      " Summary of Changes to Privacy Policy [2]\n     Comparison [3]\n\n     Revised Terms of Service [4]\n     Summary of "
                                      "Changes to Terms of Service [5]\n     Comparison [6]\n\nThanks!\n\nThe Zoho Team  \nZoho Corporation \n4141 Hacienda "
                                      "Drive Pleasanton,  \nCA 94588,USA .[7]\n\n\n--------\n[1] "
                                      "http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b3d\n[2] "
                                      "http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b3f\n[3] "
                                      "http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b41\n[4] "
                                      "http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b47\n[5] "
                                      "http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b45\n[6] "
                                      "http://zc1.maillist-manage.com/click.zc?od=11287eca375dad&repDgs=189bb4a86db70a&linkDgs=189bb4a7ed6b43\n[7] "
                                      "http://zc1.maillist-manage.com/open/od11287eca375dad_rd189bb4a86db70a.gif\n");
}

void ConvertHtmlToPlainTextTest::shouldConvertToPlainText()
{
    QFETCH(QString, inputText);
    QFETCH(QString, convertedText);

    MimeTreeParser::ConvertHtmlToPlainText convert;
    convert.setHtmlString(inputText);
    const QString result = convert.generatePlainText();
    QVERIFY(!result.isEmpty());

    QCOMPARE(result, convertedText);
}

QTEST_MAIN(ConvertHtmlToPlainTextTest)

#include "moc_converthtmltoplaintexttest.cpp"

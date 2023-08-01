/* SPDX-FileCopyrightText: 2009 Thomas McGuire <mcguire@kde.org>

   SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/
#pragma once

#include <QObject>

class StringUtilTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void test_parseMailToBug832795();
    void test_SmartQuote_data();
    void test_SmartQuote();
    void test_signatureStripping();
    void test_stripOffMessagePrefixBenchmark();
    void test_parseMailtoUrl_data();
    void test_parseMailtoUrl();
    void test_parseMailtoUrlExtra();
    void test_replaceMessagePrefix_data();
    void test_replaceMessagePrefix();
    void test_stripOffMessagePrefix_data();
    void test_stripOffMessagePrefix();
    void test_formatQuotePrefix_data();
    void test_formatQuotePrefix();
    void test_parseMailToBug366981();
    void test_parseDuplicateQueryItems();
    void test_parseMAilToBug402378();
    void test_parseMailToBug406208();
    void test_parseMailToWithUtf8Encoded();
    void test_parseMailToWithUtf8QuotedEncoded();
    void test_crashXdgemail();
    void test_xdgemail();
    void test_parseMailToBug206269();
    void test_splitAddressField();
};

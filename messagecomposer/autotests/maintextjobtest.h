/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

class MainTextJobTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    // "text/plain" tests:
    void testPlainText();
    void testWrappingErrors();

    // charset tests:
    void testCustomCharset();
    void testNoCharset();
    void testBadCharset();
    void testFallbackCharset();

    // html tests:
    void testHtml();
    void testHtmlWithImages();
};

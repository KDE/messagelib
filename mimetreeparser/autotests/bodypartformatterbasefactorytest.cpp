/*
  Copyright (c) 2017 Volker Krause <vkrause@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

#include "viewer/bodypartformatterbasefactory.h"
#include "interfaces/bodypartformatter.h"

#include <qtest.h>

using namespace MimeTreeParser;

class DummyFormatter : public Interface::BodyPartFormatter
{
};

class TestFactory : public BodyPartFormatterBaseFactory
{
public:
    void loadPlugins() override
    {
        textCalFormatter = new DummyFormatter;
        insert(QStringLiteral("TEXT/CALENDAR"), textCalFormatter, 100);
    }

    DummyFormatter *textCalFormatter = nullptr;
};

using namespace MimeTreeParser;

class BodyPartFormatterBaseFactoryTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testFactory()
    {
        TestFactory fac;
        auto l = fac.formattersForType(QStringLiteral("application/octet-stream"));
        QCOMPARE(l.size(), 1);
        const auto application_octet_stream_f = l.at(0);
        QVERIFY(application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("application/pgp-encrypted"));
        QCOMPARE(l.size(), 4);
        QVERIFY(l.at(0) != application_octet_stream_f);
        QCOMPARE(l.at(3), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("application/unknown"));
        QCOMPARE(l.size(), 1);
        QCOMPARE(l.at(0), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("text/plain"));
        QCOMPARE(l.size(), 3);
        const auto text_plain_f1 = l.at(0);
        const auto text_plain_f2 = l.at(1);
        QVERIFY(text_plain_f1);
        QVERIFY(text_plain_f2);
        QVERIFY(text_plain_f1 != text_plain_f2);
        QCOMPARE(l.at(2), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("text/calendar"));
        QCOMPARE(l.size(), 4);
        QVERIFY(fac.textCalFormatter);
        QCOMPARE(l.at(0), fac.textCalFormatter);
        QCOMPARE(l.at(1), text_plain_f1);
        QCOMPARE(l.at(2), text_plain_f2);
        QCOMPARE(l.at(3), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("text/x-vcalendar"));
        QCOMPARE(l.size(), 4);
        QCOMPARE(l.at(0), fac.textCalFormatter);
        l = fac.formattersForType(QStringLiteral("TEXT/X-VCALENDAR"));
        QCOMPARE(l.size(), 4);
        QCOMPARE(l.at(0), fac.textCalFormatter);

        l = fac.formattersForType(QStringLiteral("text/html"));
        QCOMPARE(l.size(), 4);
        QCOMPARE(l.at(1), text_plain_f1);
        QCOMPARE(l.at(2), text_plain_f2);
        QCOMPARE(l.at(3), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("text/rtf"));
        QCOMPARE(l.size(), 4);
        QCOMPARE(l.at(0), application_octet_stream_f);
        QCOMPARE(l.at(3), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("multipart/mixed"));
        QCOMPARE(l.size(), 1);
        const auto multipart_mixed_f = l.at(0);
        QVERIFY(multipart_mixed_f);

        l = fac.formattersForType(QStringLiteral("multipart/random"));
        QCOMPARE(l.size(), 1);
        QCOMPARE(l.at(0), multipart_mixed_f);

        l = fac.formattersForType(QStringLiteral("multipart/encrypted"));
        QCOMPARE(l.size(), 2);
        QVERIFY(l.at(0) != multipart_mixed_f);
        QCOMPARE(l.at(1), multipart_mixed_f);

        l = fac.formattersForType(QStringLiteral("image/png"));
        QCOMPARE(l.size(), 2);
        QCOMPARE(l.at(1), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("vendor/random"));
        QCOMPARE(l.size(), 1);
        QCOMPARE(l.at(0), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("message/rfc822"));
        QCOMPARE(l.size(), 4);
        QCOMPARE(l.at(3), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("message/news"));
        QCOMPARE(l.size(), 3); // ### news does not inherit rfc822
    }
};

QTEST_MAIN(BodyPartFormatterBaseFactoryTest)

#include "bodypartformatterbasefactorytest.moc"

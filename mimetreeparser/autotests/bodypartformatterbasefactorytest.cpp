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
        insert("text", "calendar", textCalFormatter);
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
        auto l = fac.formattersForType("application", "octet-stream");
        QCOMPARE(l.size(), 2); // ### should be one, no duplicates
        const auto application_octet_stream_f = l.at(0);
        QVERIFY(application_octet_stream_f);

        l = fac.formattersForType("application", "pgp-encrypted");
        QCOMPARE(l.size(), 2);
        QCOMPARE(l.at(1), application_octet_stream_f);

        l = fac.formattersForType("application", "unknown");
        QCOMPARE(l.size(), 1);
        QCOMPARE(l.at(0), application_octet_stream_f);

        l = fac.formattersForType("text", "plain");
        QCOMPARE(l.size(), 4); // ### should be 3, no duplicates, application/octet-stream missing
        const auto text_plain_f1 = l.at(0);
        const auto text_plain_f2 = l.at(1);
        QVERIFY(text_plain_f1);
        QVERIFY(text_plain_f2);
        QVERIFY(text_plain_f1 != text_plain_f2);

        l = fac.formattersForType("text", "calendar");
        QCOMPARE(l.size(), 3); // ### should be 4, application/octet-stream missing
        QVERIFY(fac.textCalFormatter);
        QCOMPARE(l.at(0), fac.textCalFormatter);
        QCOMPARE(l.at(1), text_plain_f1);
        QCOMPARE(l.at(2), text_plain_f2);

        l = fac.formattersForType("text", "x-vcalendar");
        QCOMPARE(l.size(), 2); // ### should be same as above, mimetype alias resolution missing

        l = fac.formattersForType("text", "html");
        QCOMPARE(l.size(), 3); // ### see above
        QCOMPARE(l.at(1), text_plain_f1);
        QCOMPARE(l.at(2), text_plain_f2);

        l = fac.formattersForType("text", "rtf");
        QCOMPARE(l.size(), 3); // ### this is octet-stream + 2x text, which makes little sense
        QCOMPARE(l.at(0), application_octet_stream_f);

        l = fac.formattersForType("multipart", "mixed");
        QCOMPARE(l.size(), 1);
        const auto multipart_mixed_f = l.at(0);
        QVERIFY(multipart_mixed_f);

        l = fac.formattersForType("multipart", "random");
        QCOMPARE(l.size(), 1);
        QCOMPARE(l.at(0), multipart_mixed_f);

        l = fac.formattersForType("multipart", "encrypted");
        QCOMPARE(l.size(), 2);
        QVERIFY(l.at(0) != multipart_mixed_f);
        QCOMPARE(l.at(1), multipart_mixed_f);

        l = fac.formattersForType("image", "png");
        QCOMPARE(l.size(), 1); // ### misses application/octet-stream fallback

        l = fac.formattersForType("vendor", "random");
        QCOMPARE(l.size(), 1);
        QCOMPARE(l.at(0), application_octet_stream_f);

        l = fac.formattersForType("message", "rfc822");
        QCOMPARE(l.size(), 2);
        QCOMPARE(l.at(1), application_octet_stream_f);

        l = fac.formattersForType("message", "news");
        QCOMPARE(l.size(), 1); // ### fails due to missing mimetype inheritance
    }
};

QTEST_MAIN(BodyPartFormatterBaseFactoryTest)

#include "bodypartformatterbasefactorytest.moc"

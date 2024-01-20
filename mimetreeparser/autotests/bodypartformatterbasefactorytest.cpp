/*
  SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "bodypartformatterfactory.h"
#include "interfaces/bodypartformatter.h"

#include <QTest>

using namespace MimeTreeParser;

class DummyFormatter : public Interface::BodyPartFormatter
{
    MessagePartPtr process(Interface::BodyPart &) const override
    {
        return {};
    }
};

class TestFactory : public BodyPartFormatterFactory
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
        QCOMPARE(l.size(), 3);
        const auto application_octet_stream_f = l.at(2);
        QVERIFY(application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("application/pgp-encrypted"));
        // shared-mime-info < 2.4 has application/pgp-encrytped as a subtype of text/plain, 2.4+ doesn't
        // depending of that we get two additional entries for text/plain formatters here
        QVERIFY(l.size() == 4 || l.size() == 6);
        QVERIFY(l.at(0) != application_octet_stream_f);
        QCOMPARE(l.at(l.size() - 1), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("application/unknown"));
        QCOMPARE(l.size(), 3);
        QCOMPARE(l.at(2), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("text/plain"));
        QCOMPARE(l.size(), 5);
        const auto text_plain_f1 = l.at(0);
        const auto text_plain_f2 = l.at(1);
        QVERIFY(text_plain_f1);
        QVERIFY(text_plain_f2);
        QVERIFY(text_plain_f1 != text_plain_f2);
        QCOMPARE(l.at(4), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("text/calendar"));
        QCOMPARE(l.size(), 6);
        QVERIFY(fac.textCalFormatter);
        QCOMPARE(l.at(0), fac.textCalFormatter);
        QCOMPARE(l.at(1), text_plain_f1);
        QCOMPARE(l.at(2), text_plain_f2);
        QCOMPARE(l.at(5), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("text/x-vcalendar"));
        QCOMPARE(l.size(), 6);
        QCOMPARE(l.at(0), fac.textCalFormatter);
        l = fac.formattersForType(QStringLiteral("TEXT/X-VCALENDAR"));
        QCOMPARE(l.size(), 6);
        QCOMPARE(l.at(0), fac.textCalFormatter);

        l = fac.formattersForType(QStringLiteral("text/html"));
        QCOMPARE(l.size(), 6);
        QCOMPARE(l.at(1), text_plain_f1);
        QCOMPARE(l.at(2), text_plain_f2);
        QCOMPARE(l.at(5), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("text/rtf"));
        QCOMPARE(l.size(), 6);
        QCOMPARE(l.at(0), application_octet_stream_f);
        QCOMPARE(l.at(5), application_octet_stream_f);

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
        QCOMPARE(l.size(), 4);
        QCOMPARE(l.at(3), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("vendor/random"));
        QCOMPARE(l.size(), 3);
        QCOMPARE(l.at(2), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("message/rfc822"));
        QCOMPARE(l.size(), 6);
        QCOMPARE(l.at(5), application_octet_stream_f);

        l = fac.formattersForType(QStringLiteral("message/news"));
        QCOMPARE(l.size(), 5); // ### news does not inherit rfc822
    }
};

QTEST_MAIN(BodyPartFormatterBaseFactoryTest)

#include "bodypartformatterbasefactorytest.moc"

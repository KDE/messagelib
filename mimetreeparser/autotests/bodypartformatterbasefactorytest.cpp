/*
  SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "bodypartformatterfactory.h"

#include "interfaces/bodypartformatter.h"

#include <QTest>

using namespace Qt::Literals::StringLiterals;
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
        insert(u"TEXT/CALENDAR"_s, std::make_unique<DummyFormatter>(), 100);
    }
};

using namespace MimeTreeParser;

class BodyPartFormatterBaseFactoryTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testFactory()
    {
        TestFactory fac;
        auto l = fac.formattersForType(u"application/octet-stream"_s);
        QCOMPARE(l.size(), 3);
        const auto &application_octet_stream_f = typeid(*l.at(2));

        l = fac.formattersForType(u"application/pgp-encrypted"_s);
        // shared-mime-info < 2.4 has application/pgp-encrytped as a subtype of text/plain, 2.4+ doesn't
        // depending of that we get two additional entries for text/plain formatters here
        QVERIFY(l.size() == 4 || l.size() == 6);
        QCOMPARE_NE(typeid(*l.at(0)), application_octet_stream_f);
        QCOMPARE(typeid(*l.at(l.size() - 1)), application_octet_stream_f);

        l = fac.formattersForType(u"application/unknown"_s);
        QCOMPARE(l.size(), 3);
        QCOMPARE(typeid(*l.at(2)), application_octet_stream_f);

        l = fac.formattersForType(u"text/plain"_s);
        QCOMPARE(l.size(), 5);
        const auto &text_plain_f1 = typeid(*l.at(0));
        const auto &text_plain_f2 = typeid(*l.at(1));
        QVERIFY(text_plain_f1 != text_plain_f2);
        QCOMPARE(typeid(*l.at(4)), application_octet_stream_f);

        l = fac.formattersForType(u"text/calendar"_s);
        QCOMPARE(l.size(), 6);
        QVERIFY(dynamic_cast<const DummyFormatter *>(l.at(0)));
        QCOMPARE(typeid(*l.at(1)), text_plain_f1);
        QCOMPARE(typeid(*l.at(2)), text_plain_f2);
        QCOMPARE(typeid(*l.at(5)), application_octet_stream_f);

        l = fac.formattersForType(u"text/x-vcalendar"_s);
        QCOMPARE(l.size(), 6);
        QVERIFY(dynamic_cast<const DummyFormatter *>(l.at(0)));
        l = fac.formattersForType(u"TEXT/X-VCALENDAR"_s);
        QCOMPARE(l.size(), 6);
        QVERIFY(dynamic_cast<const DummyFormatter *>(l.at(0)));

        l = fac.formattersForType(u"text/html"_s);
        QCOMPARE(l.size(), 6);
        QCOMPARE(typeid(*l.at(1)), text_plain_f1);
        QCOMPARE(typeid(*l.at(2)), text_plain_f2);
        QCOMPARE(typeid(*l.at(5)), application_octet_stream_f);

        l = fac.formattersForType(u"text/rtf"_s);
        QCOMPARE(l.size(), 6);
        QCOMPARE(typeid(*l.at(0)), application_octet_stream_f);
        QCOMPARE(typeid(*l.at(5)), application_octet_stream_f);

        l = fac.formattersForType(u"multipart/mixed"_s);
        QCOMPARE(l.size(), 1);
        const auto &multipart_mixed_f = typeid(*l.at(0));
        // QVERIFY(multipart_mixed_f);

        l = fac.formattersForType(u"multipart/random"_s);
        QCOMPARE(l.size(), 1);
        QCOMPARE(typeid(*l.at(0)), multipart_mixed_f);

        l = fac.formattersForType(u"multipart/encrypted"_s);
        QCOMPARE(l.size(), 2);
        QVERIFY(typeid(*l.at(0)) != multipart_mixed_f);
        QCOMPARE(typeid(*l.at(1)), multipart_mixed_f);

        l = fac.formattersForType(u"image/png"_s);
        QCOMPARE(l.size(), 4);
        QCOMPARE(typeid(*l.at(3)), application_octet_stream_f);

        l = fac.formattersForType(u"vendor/random"_s);
        QCOMPARE(l.size(), 3);
        QCOMPARE(typeid(*l.at(2)), application_octet_stream_f);

        l = fac.formattersForType(u"message/rfc822"_s);
        QCOMPARE(l.size(), 6);
        QCOMPARE(typeid(*l.at(5)), application_octet_stream_f);

        l = fac.formattersForType(u"message/news"_s);
        QCOMPARE(l.size(), 5); // ### news does not inherit rfc822
    }
};

QTEST_MAIN(BodyPartFormatterBaseFactoryTest)

#include "bodypartformatterbasefactorytest.moc"

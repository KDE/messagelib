/*
  SPDX-FileCopyrightText: 2014-2025 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "mailinglisttest.h"
using namespace Qt::Literals::StringLiterals;

#include "messagecore_debug.h"
#include "misc/mailinglist.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <QTest>

// TODO add test for static MailingList detect(  const std::shared_ptr<KMime::Message> &message ); and static QString name( ... );

MailingListTest::MailingListTest(QObject *parent)
    : QObject(parent)
{
}

MailingListTest::~MailingListTest() = default;

void MailingListTest::shouldHaveDefaultValue()
{
    MessageCore::MailingList ml;
    QVERIFY(ml.postUrls().isEmpty());
    QVERIFY(ml.subscribeUrls().isEmpty());
    QVERIFY(ml.unsubscribeUrls().isEmpty());
    QVERIFY(ml.helpUrls().isEmpty());
    QVERIFY(ml.archiveUrls().isEmpty());
    QVERIFY(ml.ownerUrls().isEmpty());
    QVERIFY(ml.archivedAtUrls().isEmpty());
    QVERIFY(ml.id().isEmpty());
    QCOMPARE(ml.features(), MessageCore::MailingList::None);
    QCOMPARE(ml.handler(), MessageCore::MailingList::KMail);
}

void MailingListTest::shouldRestoreFromSettings()
{
    MessageCore::MailingList ml;
    QList<QUrl> lst;
    lst << QUrl(u"http://www.kde.org"_s) << QUrl(u"http://www.koffice.org"_s);
    ml.setPostUrls(lst);
    lst << QUrl(u"mailto://www.kde2.org"_s) << QUrl(u"http://www.koffice2.org"_s);
    ml.setSubscribeUrls(lst);
    lst << QUrl(u"mailto://www.kde3.org"_s) << QUrl(u"http://www.koffice3.org"_s);
    ml.setUnsubscribeUrls(lst);
    lst << QUrl(u"mailto://www.kde4.org"_s) << QUrl(u"http://www.koffice4.org"_s);
    ml.setHelpUrls(lst);
    /* Note: mArchivedAtUrl deliberately not saved here as it refers to a single
     * instance of a message rather than an element of a general mailing list.
     * http://reviewboard.kde.org/r/1768/#review2783
     */
    // normal that we don't save it.
    // ml.setArchivedAtUrls(lst);
    lst << QUrl(u"mailto://www.kde5.org"_s) << QUrl(u"http://www.koffice5.org"_s);
    ml.setArchiveUrls(lst);
    lst << QUrl(u"mailto://www.kde6.org"_s) << QUrl(u"http://www.koffice6.org"_s);
    ml.setOwnerUrls(lst);
    ml.setId(u"ID"_s);
    ml.setHandler(MessageCore::MailingList::Browser);

    KConfigGroup grp(KSharedConfig::openConfig(), u"testsettings"_s);
    ml.writeConfig(grp);

    MessageCore::MailingList restoreMl;
    restoreMl.readConfig(grp);
    QCOMPARE(ml, restoreMl);
}

void MailingListTest::shouldCopyReminderInfo()
{
    MessageCore::MailingList ml;
    QList<QUrl> lst;
    lst << QUrl(u"http://www.kde.org"_s) << QUrl(u"http://www.koffice.org"_s);
    ml.setPostUrls(lst);
    lst << QUrl(u"http://www.kde2.org"_s) << QUrl(u"http://www.koffice2.org"_s);
    ml.setSubscribeUrls(lst);
    lst << QUrl(u"http://www.kde3.org"_s) << QUrl(u"http://www.koffice3.org"_s);
    ml.setUnsubscribeUrls(lst);
    lst << QUrl(u"http://www.kde4.org"_s) << QUrl(u"http://www.koffice4.org"_s);
    ml.setHelpUrls(lst);
    lst << QUrl(u"http://www.kde5.org"_s) << QUrl(u"http://www.koffice5.org"_s);
    ml.setArchivedAtUrls(lst);
    lst << QUrl(u"http://www.kde5.org"_s) << QUrl(u"http://www.koffice6.org"_s);
    ml.setArchiveUrls(lst);
    lst << QUrl(u"http://www.kde6.org"_s) << QUrl(u"http://www.koffice6.org"_s);
    ml.setOwnerUrls(lst);
    ml.setPostUrls(lst);
    ml.setSubscribeUrls(lst);
    ml.setUnsubscribeUrls(lst);
    ml.setHelpUrls(lst);
    ml.setArchivedAtUrls(lst);
    ml.setArchiveUrls(lst);
    ml.setOwnerUrls(lst);
    ml.setId(u"ID"_s);
    ml.setHandler(MessageCore::MailingList::Browser);

    MessageCore::MailingList restoreMl(ml);
    QCOMPARE(ml, restoreMl);
}

QTEST_APPLESS_MAIN(MailingListTest)

#include "moc_mailinglisttest.cpp"

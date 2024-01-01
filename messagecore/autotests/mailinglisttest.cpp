/*
  SPDX-FileCopyrightText: 2014-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-only
*/

#include "mailinglisttest.h"
#include "messagecore_debug.h"
#include "misc/mailinglist.h"
#include <KConfigGroup>
#include <KSharedConfig>
#include <QTest>

// TODO add test for static MailingList detect(  const KMime::Message::Ptr &message ); and static QString name( ... );

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
    QVERIFY(ml.features() == MessageCore::MailingList::None);
    QVERIFY(ml.handler() == MessageCore::MailingList::KMail);
}

void MailingListTest::shouldRestoreFromSettings()
{
    MessageCore::MailingList ml;
    QList<QUrl> lst;
    lst << QUrl(QStringLiteral("http://www.kde.org")) << QUrl(QStringLiteral("http://www.koffice.org"));
    ml.setPostUrls(lst);
    lst << QUrl(QStringLiteral("mailto://www.kde2.org")) << QUrl(QStringLiteral("http://www.koffice2.org"));
    ml.setSubscribeUrls(lst);
    lst << QUrl(QStringLiteral("mailto://www.kde3.org")) << QUrl(QStringLiteral("http://www.koffice3.org"));
    ml.setUnsubscribeUrls(lst);
    lst << QUrl(QStringLiteral("mailto://www.kde4.org")) << QUrl(QStringLiteral("http://www.koffice4.org"));
    ml.setHelpUrls(lst);
    /* Note: mArchivedAtUrl deliberately not saved here as it refers to a single
     * instance of a message rather than an element of a general mailing list.
     * http://reviewboard.kde.org/r/1768/#review2783
     */
    // normal that we don't save it.
    // ml.setArchivedAtUrls(lst);
    lst << QUrl(QStringLiteral("mailto://www.kde5.org")) << QUrl(QStringLiteral("http://www.koffice5.org"));
    ml.setArchiveUrls(lst);
    lst << QUrl(QStringLiteral("mailto://www.kde6.org")) << QUrl(QStringLiteral("http://www.koffice6.org"));
    ml.setOwnerUrls(lst);
    ml.setId(QStringLiteral("ID"));
    ml.setHandler(MessageCore::MailingList::Browser);

    KConfigGroup grp(KSharedConfig::openConfig(), QStringLiteral("testsettings"));
    ml.writeConfig(grp);

    MessageCore::MailingList restoreMl;
    restoreMl.readConfig(grp);
    QCOMPARE(ml, restoreMl);
}

void MailingListTest::shouldCopyReminderInfo()
{
    MessageCore::MailingList ml;
    QList<QUrl> lst;
    lst << QUrl(QStringLiteral("http://www.kde.org")) << QUrl(QStringLiteral("http://www.koffice.org"));
    ml.setPostUrls(lst);
    lst << QUrl(QStringLiteral("http://www.kde2.org")) << QUrl(QStringLiteral("http://www.koffice2.org"));
    ml.setSubscribeUrls(lst);
    lst << QUrl(QStringLiteral("http://www.kde3.org")) << QUrl(QStringLiteral("http://www.koffice3.org"));
    ml.setUnsubscribeUrls(lst);
    lst << QUrl(QStringLiteral("http://www.kde4.org")) << QUrl(QStringLiteral("http://www.koffice4.org"));
    ml.setHelpUrls(lst);
    lst << QUrl(QStringLiteral("http://www.kde5.org")) << QUrl(QStringLiteral("http://www.koffice5.org"));
    ml.setArchivedAtUrls(lst);
    lst << QUrl(QStringLiteral("http://www.kde5.org")) << QUrl(QStringLiteral("http://www.koffice6.org"));
    ml.setArchiveUrls(lst);
    lst << QUrl(QStringLiteral("http://www.kde6.org")) << QUrl(QStringLiteral("http://www.koffice6.org"));
    ml.setOwnerUrls(lst);
    ml.setPostUrls(lst);
    ml.setSubscribeUrls(lst);
    ml.setUnsubscribeUrls(lst);
    ml.setHelpUrls(lst);
    ml.setArchivedAtUrls(lst);
    ml.setArchiveUrls(lst);
    ml.setOwnerUrls(lst);
    ml.setId(QStringLiteral("ID"));
    ml.setHandler(MessageCore::MailingList::Browser);

    MessageCore::MailingList restoreMl(ml);
    QCOMPARE(ml, restoreMl);
}

QTEST_APPLESS_MAIN(MailingListTest)

#include "moc_mailinglisttest.cpp"

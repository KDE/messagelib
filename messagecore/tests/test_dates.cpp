/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */
#include <MessageCore/DateFormatter>
using namespace Qt::Literals::StringLiterals;

#include <QDebug>

using namespace MessageCore;

#ifndef Q_OS_WIN
static void initLocale()
{
    setenv("LC_ALL", "en_US.utf-8", 1);
    setenv("TZ", "UTC", 1);
}

Q_CONSTRUCTOR_FUNCTION(initLocale)
#endif

int main()
{
    DateFormatter t;

    auto ntime = QDateTime::currentDateTime();
    qDebug() << "Time now:";
    qDebug() << "tFancy : \t" << t.dateString(ntime);
    t.setFormat(DateFormatter::Localized);
    qDebug() << "tLocalized : \t" << t.dateString(ntime);
    t.setFormat(DateFormatter::CTime);
    qDebug() << "tCTime : \t" << t.dateString(ntime);

    t.setCustomFormat(u"MMMM dddd yyyy Z"_s);
    qDebug() << "tCustom : \t" << t.dateString(ntime);

    ntime = ntime.addSecs(24 * 3600 + 1);
    qDebug() << "Time 24 hours and 1 second ago:";
    t.setFormat(DateFormatter::Fancy);
    qDebug() << "tFancy : \t" << t.dateString(ntime);
    t.setFormat(DateFormatter::Localized);
    qDebug() << "tLocalized : \t" << t.dateString(ntime);
    t.setFormat(DateFormatter::CTime);
    qDebug() << "tCTime : \t" << t.dateString(ntime);
    t.setCustomFormat(u"MMMM dddd Z yyyy"_s);
    qDebug() << "tCustom : \t" << t.dateString(ntime);

    t.setFormat(DateFormatter::Fancy);
    ntime = ntime.addSecs(24 * 3600 * 30 + 59);
    qDebug() << "Time 31 days and 1 minute ago:";
    qDebug() << "tFancy : \t" << t.dateString(ntime);
    t.setFormat(DateFormatter::Localized);
    qDebug() << "tLocalized : \t" << t.dateString(ntime);
    t.setFormat(DateFormatter::CTime);
    qDebug() << "tCTime : \t" << t.dateString(ntime);
    t.setCustomFormat(u"MMMM Z dddd yyyy"_s);
    qDebug() << "tCustom : \t" << t.dateString(ntime);

    qDebug() << "Static functions (dates like in the last test):";
    qDebug() << "tFancy : \t" << DateFormatter::formatDate(DateFormatter::Fancy, ntime);
    qDebug() << "tLocalized : \t" << DateFormatter::formatDate(DateFormatter::Localized, ntime);
    qDebug() << "tCTime : \t" << DateFormatter::formatDate(DateFormatter::CTime, ntime);
    qDebug() << "tCustom : \t" << DateFormatter::formatDate(DateFormatter::Custom, ntime, u"Z MMMM dddd yyyy"_s);
    t.setFormat(DateFormatter::Fancy);
    qDebug() << "QDateTime taking: (dates as in first test)";
    qDebug() << "tFancy : \t" << t.dateString((QDateTime::currentDateTime()));
    t.setFormat(DateFormatter::Localized);
    qDebug() << "tLocalized : \t" << t.dateString(QDateTime::currentDateTime());
    t.setFormat(DateFormatter::CTime);
    qDebug() << "tCTime : \t" << t.dateString(QDateTime::currentDateTime());
    t.setCustomFormat(u"MMMM d dddd yyyy Z"_s);
    qDebug() << "tCustom : \t" << t.dateString(QDateTime::currentDateTime());
}

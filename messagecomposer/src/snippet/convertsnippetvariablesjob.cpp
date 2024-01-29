/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "convertsnippetvariablesjob.h"
#include "composer/composerviewinterface.h"
#include "snippet/convertsnippetvariablesutil.h"
#include <KEmailAddress>
#include <KMime/Types>
#include <QDebug>
#include <TemplateParser/TemplatesUtil>
using namespace MessageComposer;
ConvertSnippetVariablesJob::ConvertSnippetVariablesJob(QObject *parent)
    : QObject(parent)
{
}

ConvertSnippetVariablesJob::~ConvertSnippetVariablesJob()
{
    delete mComposerViewInterface;
}

void ConvertSnippetVariablesJob::setText(const QString &str)
{
    mText = str;
}

bool ConvertSnippetVariablesJob::canStart() const
{
    if (mText.isEmpty() || !mComposerViewInterface) {
        return false;
    }
    return true;
}

void ConvertSnippetVariablesJob::start()
{
    if (!canStart()) {
        Q_EMIT textConverted(QString());
        deleteLater();
        return;
    }
    Q_EMIT textConverted(convertVariables(mComposerViewInterface, mText));
    deleteLater();
}

QString ConvertSnippetVariablesJob::text() const
{
    return mText;
}

MessageComposer::ComposerViewInterface *ConvertSnippetVariablesJob::composerViewInterface() const
{
    return mComposerViewInterface;
}

void ConvertSnippetVariablesJob::setComposerViewInterface(MessageComposer::ComposerViewInterface *composerViewInterface)
{
    mComposerViewInterface = composerViewInterface;
}

QString ConvertSnippetVariablesJob::convertVariables(const QString &cmd, int &i, QChar c)
{
    QString result;
    if (cmd.startsWith(QLatin1StringView("LASTYEAR"))) {
        i += strlen("LASTYEAR");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::lastYear());
    } else if (cmd.startsWith(QLatin1StringView("NEXTYEAR"))) {
        i += strlen("NEXTYEAR");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::nextYear());
    } else if (cmd.startsWith(QLatin1StringView("MONTHNUMBER"))) {
        i += strlen("MONTHNUMBER");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::monthNumber());
    } else if (cmd.startsWith(QLatin1StringView("DAYOFMONTH"))) {
        i += strlen("DAYOFMONTH");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::dayOfMonth());
    } else if (cmd.startsWith(QLatin1StringView("WEEKNUMBER"))) {
        i += strlen("WEEKNUMBER");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::weekNumber());
    } else if (cmd.startsWith(QLatin1StringView("MONTHNAMESHORT"))) {
        i += strlen("MONTHNAMESHORT");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::monthNameShort());
    } else if (cmd.startsWith(QLatin1StringView("MONTHNAMELONG"))) {
        i += strlen("MONTHNAMELONG");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::monthNameLong());
    } else if (cmd.startsWith(QLatin1StringView("DAYOFWEEKNAMESHORT"))) {
        i += strlen("DAYOFWEEKNAMESHORT");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::dayOfWeekNameShort());
    } else if (cmd.startsWith(QLatin1StringView("DAYOFWEEKNAMELONG"))) {
        i += strlen("DAYOFWEEKNAMELONG");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::dayOfWeekNameLong());
    } else if (cmd.startsWith(QLatin1StringView("YEARLASTMONTH"))) {
        i += strlen("YEARLASTMONTH");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::yearLastMonth());
    } else if (cmd.startsWith(QLatin1StringView("YEAR"))) {
        i += strlen("YEAR");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::year());
    } else if (cmd.startsWith(QLatin1StringView("DAYOFWEEK"))) {
        i += strlen("DAYOFWEEK");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::dayOfWeek());
    } else if (cmd.startsWith(QLatin1StringView("LASTMONTHNAMELONG"))) {
        i += strlen("LASTMONTHNAMELONG");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::lastMonthNameLong());
    } else {
        result.append(c);
    }
    return result;
}

QString ConvertSnippetVariablesJob::convertVariables(MessageComposer::ComposerViewInterface *composerView, const QString &text)
{
    QString result;
    const int tmpl_len = text.length();
    for (int i = 0; i < tmpl_len; ++i) {
        const QChar c = text[i];
        if (c == QLatin1Char('%')) {
            const QString cmd = text.mid(i + 1);
            if (composerView) {
                if (cmd.startsWith(QLatin1StringView("CCADDR"))) {
                    i += strlen("CCADDR");
                    const QString str = composerView->cc();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("CCFNAME"))) {
                    i += strlen("CCFNAME");
                    const QString str = getFirstNameFromEmail(composerView->cc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("CCLNAME"))) {
                    i += strlen("CCLNAME");
                    const QString str = getLastNameFromEmail(composerView->cc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("CCNAME"))) {
                    i += strlen("CCNAME");
                    const QString str = getNameFromEmail(composerView->cc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("BCCADDR"))) {
                    i += strlen("BCCADDR");
                    const QString str = composerView->bcc();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("BCCFNAME"))) {
                    i += strlen("BCCFNAME");
                    const QString str = getFirstNameFromEmail(composerView->bcc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("BCCLNAME"))) {
                    i += strlen("BCCLNAME");
                    const QString str = getLastNameFromEmail(composerView->bcc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("BCCNAME"))) {
                    i += strlen("BCCNAME");
                    const QString str = getNameFromEmail(composerView->bcc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("FULLSUBJECT"))) {
                    i += strlen("FULLSUBJECT");
                    const QString str = composerView->subject();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("TOADDR"))) {
                    i += strlen("TOADDR");
                    const QString str = composerView->to();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("TOFNAME"))) {
                    i += strlen("TOFNAME");
                    const QString str = getFirstNameFromEmail(composerView->to());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("TOLNAME"))) {
                    i += strlen("TOLNAME");
                    const QString str = getLastNameFromEmail(composerView->to());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("TONAME"))) {
                    i += strlen("TONAME");
                    const QString str = getNameFromEmail(composerView->to());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("FROMADDR"))) {
                    i += strlen("FROMADDR");
                    const QString str = composerView->from();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("FROMFNAME"))) {
                    i += strlen("FROMFNAME");
                    const QString str = getFirstNameFromEmail(composerView->from());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("FROMLNAME"))) {
                    i += strlen("FROMLNAME");
                    const QString str = getLastNameFromEmail(composerView->from());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("FROMNAME"))) {
                    i += strlen("FROMNAME");
                    const QString str = getNameFromEmail(composerView->from());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("DOW"))) {
                    i += strlen("DOW");
                    const QString str = composerView->insertDayOfWeek();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("DATE"))) {
                    i += strlen("DATE");
                    const QString str = composerView->longDate();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("SHORTDATE"))) {
                    i += strlen("SHORTDATE");
                    const QString str = composerView->shortDate();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("TIME"))) {
                    i += strlen("TIME");
                    const QString str = composerView->shortTime();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("TIMELONG"))) {
                    i += strlen("TIMELONG");
                    const QString str = composerView->longTime();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("ATTACHMENTCOUNT"))) {
                    i += strlen("ATTACHMENTCOUNT");
                    const QString str = QString::number(composerView->attachments().count());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("ATTACHMENTNAMES"))) {
                    i += strlen("ATTACHMENTNAMES");
                    const QString str = composerView->attachments().names().join(QLatin1Char(','));
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("ATTACHMENTFILENAMES"))) {
                    i += strlen("ATTACHMENTFILENAMES");
                    const QString str = composerView->attachments().fileNames().join(QLatin1Char(','));
                    result.append(str);
                } else if (cmd.startsWith(QLatin1StringView("ATTACHMENTNAMESANDSIZES"))) {
                    i += strlen("ATTACHMENTNAMESANDSIZES");
                    const QString str = composerView->attachments().namesAndSize().join(QLatin1Char(','));
                    result.append(str);
                } else {
                    result.append(convertVariables(cmd, i, c));
                }
            } else {
                result.append(convertVariables(cmd, i, c));
            }
        } else {
            result.append(c);
        }
    }
    return result;
}

QString ConvertSnippetVariablesJob::getNameFromEmail(const QString &address)
{
    const QStringList lst = address.split(QStringLiteral(", "));
    QStringList resultName;
    for (const QString &str : lst) {
        KMime::Types::Mailbox mailBoxAddress;
        mailBoxAddress.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(str));
        const QString firstName = mailBoxAddress.name();
        if (!firstName.isEmpty()) {
            resultName << firstName;
        }
    }

    const QString str = resultName.isEmpty() ? QString() : resultName.join(QStringLiteral(", "));
    return str;
}

QString ConvertSnippetVariablesJob::getFirstNameFromEmail(const QString &address)
{
    const QStringList lst = KEmailAddress::splitAddressList(address);
    QStringList resultName;
    for (const QString &str : lst) {
        KMime::Types::Mailbox mailBoxAddress;
        mailBoxAddress.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(str));
        const QString firstName = TemplateParser::Util::getFirstNameFromEmail(mailBoxAddress.name());
        if (!firstName.isEmpty()) {
            resultName << firstName;
        }
    }

    const QString str = resultName.isEmpty() ? QString() : resultName.join(QStringLiteral(", "));
    return str;
}

QString ConvertSnippetVariablesJob::getLastNameFromEmail(const QString &address)
{
    const QStringList lst = KEmailAddress::splitAddressList(address);
    QStringList resultName;
    for (const QString &str : lst) {
        KMime::Types::Mailbox newAddress;
        newAddress.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(str));
        qDebug() << "newAddress.name()  " << newAddress.name();
        const QString lastName = TemplateParser::Util::getLastNameFromEmail(newAddress.name());
        if (!lastName.isEmpty()) {
            resultName << lastName;
        }
    }

    const QString str = resultName.isEmpty() ? QString() : resultName.join(QStringLiteral(", "));
    return str;
}

#include "moc_convertsnippetvariablesjob.cpp"

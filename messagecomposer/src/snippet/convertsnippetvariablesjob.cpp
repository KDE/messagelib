/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "convertsnippetvariablesjob.h"
#include "composer/composerviewinterface.h"
#include "messagecomposer_debug.h"
#include "snippet/convertsnippetvariablesutil.h"
#include <KEmailAddress>
#include <KMime/Types>
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
    if (cmd.startsWith(QLatin1String("LASTYEAR"))) {
        i += strlen("LASTYEAR");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::lastYear());
    } else if (cmd.startsWith(QLatin1String("NEXTYEAR"))) {
        i += strlen("NEXTYEAR");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::nextYear());
    } else if (cmd.startsWith(QLatin1String("MONTHNUMBER"))) {
        i += strlen("MONTHNUMBER");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::monthNumber());
    } else if (cmd.startsWith(QLatin1String("DAYOFMONTH"))) {
        i += strlen("DAYOFMONTH");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::dayOfMonth());
    } else if (cmd.startsWith(QLatin1String("WEEKNUMBER"))) {
        i += strlen("WEEKNUMBER");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::weekNumber());
    } else if (cmd.startsWith(QLatin1String("MONTHNAMESHORT"))) {
        i += strlen("MONTHNAMESHORT");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::monthNameShort());
    } else if (cmd.startsWith(QLatin1String("MONTHNAMELONG"))) {
        i += strlen("MONTHNAMELONG");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::monthNameLong());
    } else if (cmd.startsWith(QLatin1String("DAYOFWEEKNAMESHORT"))) {
        i += strlen("DAYOFWEEKNAMESHORT");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::dayOfWeekNameShort());
    } else if (cmd.startsWith(QLatin1String("DAYOFWEEKNAMELONG"))) {
        i += strlen("DAYOFWEEKNAMELONG");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::dayOfWeekNameLong());
    } else if (cmd.startsWith(QLatin1String("YEARLASTMONTH"))) {
        i += strlen("YEARLASTMONTH");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::yearLastMonth());
    } else if (cmd.startsWith(QLatin1String("YEAR"))) {
        i += strlen("YEAR");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::year());
    } else if (cmd.startsWith(QLatin1String("DAYOFWEEK"))) {
        i += strlen("DAYOFWEEK");
        result.append(MessageComposer::ConvertSnippetVariablesUtil::dayOfWeek());
    } else if (cmd.startsWith(QLatin1String("LASTMONTHNAMELONG"))) {
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
                if (cmd.startsWith(QLatin1String("CCADDR"))) {
                    i += strlen("CCADDR");
                    const QString str = composerView->cc();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("CCFNAME"))) {
                    i += strlen("CCFNAME");
                    const QString str = getFirstNameFromEmail(composerView->cc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("CCLNAME"))) {
                    i += strlen("CCLNAME");
                    const QString str = getLastNameFromEmail(composerView->cc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("CCNAME"))) {
                    i += strlen("CCNAME");
                    const QString str = getNameFromEmail(composerView->cc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("BCCADDR"))) {
                    i += strlen("BCCADDR");
                    const QString str = composerView->bcc();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("BCCFNAME"))) {
                    i += strlen("BCCFNAME");
                    const QString str = getFirstNameFromEmail(composerView->bcc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("BCCLNAME"))) {
                    i += strlen("BCCLNAME");
                    const QString str = getLastNameFromEmail(composerView->bcc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("BCCNAME"))) {
                    i += strlen("BCCNAME");
                    const QString str = getNameFromEmail(composerView->bcc());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("FULLSUBJECT"))) {
                    i += strlen("FULLSUBJECT");
                    const QString str = composerView->subject();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("TOADDR"))) {
                    i += strlen("TOADDR");
                    const QString str = composerView->to();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("TOFNAME"))) {
                    i += strlen("TOFNAME");
                    const QString str = getFirstNameFromEmail(composerView->to());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("TOLNAME"))) {
                    i += strlen("TOLNAME");
                    const QString str = getLastNameFromEmail(composerView->to());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("TONAME"))) {
                    i += strlen("TONAME");
                    const QString str = getNameFromEmail(composerView->to());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("FROMADDR"))) {
                    i += strlen("FROMADDR");
                    const QString str = composerView->from();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("FROMFNAME"))) {
                    i += strlen("FROMFNAME");
                    const QString str = getFirstNameFromEmail(composerView->from());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("FROMLNAME"))) {
                    i += strlen("FROMLNAME");
                    const QString str = getLastNameFromEmail(composerView->from());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("FROMNAME"))) {
                    i += strlen("FROMNAME");
                    const QString str = getNameFromEmail(composerView->from());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("DOW"))) {
                    i += strlen("DOW");
                    const QString str = composerView->insertDayOfWeek();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("DATE"))) {
                    i += strlen("DATE");
                    const QString str = composerView->longDate();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("SHORTDATE"))) {
                    i += strlen("SHORTDATE");
                    const QString str = composerView->shortDate();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("TIME"))) {
                    i += strlen("TIME");
                    const QString str = composerView->shortTime();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("TIMELONG"))) {
                    i += strlen("TIMELONG");
                    const QString str = composerView->longTime();
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("ATTACHMENTCOUNT"))) {
                    i += strlen("ATTACHMENTCOUNT");
                    const QString str = QString::number(composerView->attachments().count());
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("ATTACHMENTNAMES"))) {
                    i += strlen("ATTACHMENTNAMES");
                    const QString str = composerView->attachments().names().join(QLatin1Char(','));
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("ATTACHMENTFILENAMES"))) {
                    i += strlen("ATTACHMENTFILENAMES");
                    const QString str = composerView->attachments().fileNames().join(QLatin1Char(','));
                    result.append(str);
                } else if (cmd.startsWith(QLatin1String("ATTACHMENTNAMESANDSIZES"))) {
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
        KMime::Types::Mailbox address;
        address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(str));
        const QString firstName = address.name();
        if (!firstName.isEmpty()) {
            resultName << firstName;
        }
    }

    const QString str = resultName.isEmpty() ? QString() : resultName.join(QStringLiteral(", "));
    return str;
}

QString ConvertSnippetVariablesJob::getFirstNameFromEmail(const QString &address)
{
    const QStringList lst = address.split(QStringLiteral(", "));
    QStringList resultName;
    for (const QString &str : lst) {
        KMime::Types::Mailbox address;
        address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(str));
        const QString firstName = TemplateParser::Util::getFirstNameFromEmail(address.name());
        if (!firstName.isEmpty()) {
            resultName << firstName;
        }
    }

    const QString str = resultName.isEmpty() ? QString() : resultName.join(QStringLiteral(", "));
    return str;
}

QString ConvertSnippetVariablesJob::getLastNameFromEmail(const QString &address)
{
    const QStringList lst = address.split(QStringLiteral(", "));
    QStringList resultName;
    for (const QString &str : lst) {
        KMime::Types::Mailbox address;
        address.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(str));
        const QString lastName = TemplateParser::Util::getLastNameFromEmail(address.name());
        if (!lastName.isEmpty()) {
            resultName << lastName;
        }
    }

    const QString str = resultName.isEmpty() ? QString() : resultName.join(QStringLiteral(", "));
    return str;
}

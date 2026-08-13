/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "convertsnippetvariablesjob.h"
#include "composer/composerviewinterface.h"
#include "snippet/convertsnippetvariablesutil.h"
#include <KEmailAddress>
#include <KMime/Types>
#include <TemplateParser/TemplatesUtil>
using namespace MessageComposer;
using namespace Qt::Literals::StringLiterals;

namespace
{
template<typename Extract>
QString namesFromEmail(const QString &address, Extract &&extract)
{
    const QStringList lst = KEmailAddress::splitAddressList(address);
    QStringList resultName;
    resultName.reserve(lst.count());
    for (const QString &str : lst) {
        KMime::Types::Mailbox mailBoxAddress;
        mailBoxAddress.fromUnicodeString(KEmailAddress::normalizeAddressesAndEncodeIdn(str));
        const QString name = extract(mailBoxAddress.name());
        if (!name.isEmpty()) {
            resultName << name;
        }
    }
    return resultName.join(u", "_s);
}

QString nameFromEmail(const QString &address)
{
    return namesFromEmail(address, [](const QString &name) {
        return name;
    });
}

QString firstNameFromEmail(const QString &address)
{
    return namesFromEmail(address, [](const QString &name) {
        return TemplateParser::Util::getFirstNameFromEmail(name);
    });
}

QString lastNameFromEmail(const QString &address)
{
    return namesFromEmail(address, [](const QString &name) {
        return TemplateParser::Util::getLastNameFromEmail(name);
    });
}

struct SnippetVariable {
    // Variable name, without the leading '%'.
    QString name;
    // Variables which need a composer view are left as-is when we don't have one.
    bool requiresComposerView = false;
    QString (*value)(ComposerViewInterface *composerView) = nullptr;
};

const QList<SnippetVariable> &snippetVariables()
{
    using namespace MessageComposer::ConvertSnippetVariablesUtil;
    static const QList<SnippetVariable> variables = [] {
        QList<SnippetVariable> list;
        // snippetVariableFromEnum() is the single source of truth for the variable names.
        const auto add = [&list](VariableType type, bool requiresComposerView, QString (*value)(ComposerViewInterface *)) {
            list.append({snippetVariableFromEnum(type).mid(1), requiresComposerView, value});
        };
        add(CcAddr, true, [](ComposerViewInterface *composerView) {
            return composerView->cc();
        });
        add(CcFname, true, [](ComposerViewInterface *composerView) {
            return firstNameFromEmail(composerView->cc());
        });
        add(CcLname, true, [](ComposerViewInterface *composerView) {
            return lastNameFromEmail(composerView->cc());
        });
        add(CcName, true, [](ComposerViewInterface *composerView) {
            return nameFromEmail(composerView->cc());
        });
        add(BccAddr, true, [](ComposerViewInterface *composerView) {
            return composerView->bcc();
        });
        add(BccFname, true, [](ComposerViewInterface *composerView) {
            return firstNameFromEmail(composerView->bcc());
        });
        add(BccLname, true, [](ComposerViewInterface *composerView) {
            return lastNameFromEmail(composerView->bcc());
        });
        add(BccName, true, [](ComposerViewInterface *composerView) {
            return nameFromEmail(composerView->bcc());
        });
        add(ToAddr, true, [](ComposerViewInterface *composerView) {
            return composerView->to();
        });
        add(ToFname, true, [](ComposerViewInterface *composerView) {
            return firstNameFromEmail(composerView->to());
        });
        add(ToLname, true, [](ComposerViewInterface *composerView) {
            return lastNameFromEmail(composerView->to());
        });
        add(ToName, true, [](ComposerViewInterface *composerView) {
            return nameFromEmail(composerView->to());
        });
        add(FromAddr, true, [](ComposerViewInterface *composerView) {
            return composerView->from();
        });
        add(FromFname, true, [](ComposerViewInterface *composerView) {
            return firstNameFromEmail(composerView->from());
        });
        add(FromLname, true, [](ComposerViewInterface *composerView) {
            return lastNameFromEmail(composerView->from());
        });
        add(FromName, true, [](ComposerViewInterface *composerView) {
            return nameFromEmail(composerView->from());
        });
        add(FullSubject, true, [](ComposerViewInterface *composerView) {
            return composerView->subject();
        });
        add(Dow, true, [](ComposerViewInterface *composerView) {
            return composerView->insertDayOfWeek();
        });
        add(Date, true, [](ComposerViewInterface *composerView) {
            return composerView->longDate();
        });
        add(ShortDate, true, [](ComposerViewInterface *composerView) {
            return composerView->shortDate();
        });
        add(Time, true, [](ComposerViewInterface *composerView) {
            return composerView->shortTime();
        });
        add(TimeLong, true, [](ComposerViewInterface *composerView) {
            return composerView->longTime();
        });
        add(AttachmentCount, true, [](ComposerViewInterface *composerView) {
            return QString::number(composerView->attachments().count());
        });
        add(AttachmentName, true, [](ComposerViewInterface *composerView) {
            return composerView->attachments().names().join(u',');
        });
        add(AttachmentFilenames, true, [](ComposerViewInterface *composerView) {
            return composerView->attachments().fileNames().join(u',');
        });
        add(AttachmentNamesAndSizes, true, [](ComposerViewInterface *composerView) {
            return composerView->attachments().namesAndSize().join(u',');
        });
        add(Year, false, [](ComposerViewInterface *) {
            return year();
        });
        add(LastYear, false, [](ComposerViewInterface *) {
            return lastYear();
        });
        add(NextYear, false, [](ComposerViewInterface *) {
            return nextYear();
        });
        add(YearLastMonth, false, [](ComposerViewInterface *) {
            return yearLastMonth();
        });
        add(MonthNumber, false, [](ComposerViewInterface *) {
            return monthNumber();
        });
        add(MonthNameShort, false, [](ComposerViewInterface *) {
            return monthNameShort();
        });
        add(MonthNameLong, false, [](ComposerViewInterface *) {
            return monthNameLong();
        });
        add(LastMonthNameLong, false, [](ComposerViewInterface *) {
            return lastMonthNameLong();
        });
        add(WeekNumber, false, [](ComposerViewInterface *) {
            return weekNumber();
        });
        add(DayNumber, false, [](ComposerViewInterface *) {
            return dayNumber();
        });
        add(DayOfMonth, false, [](ComposerViewInterface *) {
            return dayOfMonth();
        });
        add(DayOfWeek, false, [](ComposerViewInterface *) {
            return dayOfWeek();
        });
        add(DayOfWeekNameShort, false, [](ComposerViewInterface *) {
            return dayOfWeekNameShort();
        });
        add(DayOfWeekNameLong, false, [](ComposerViewInterface *) {
            return dayOfWeekNameLong();
        });
        add(CustomDate, false, [](ComposerViewInterface *) {
            return customDate();
        });
        return list;
    }();
    return variables;
}

const SnippetVariable *findSnippetVariable(QStringView cmd, bool hasComposerView)
{
    const SnippetVariable *found = nullptr;
    for (const SnippetVariable &variable : snippetVariables()) {
        if (variable.requiresComposerView && !hasComposerView) {
            continue;
        }
        // The longest name wins, so the order of the variables doesn't matter (%TIMELONG vs %TIME).
        if ((!found || variable.name.size() > found->name.size()) && cmd.startsWith(variable.name)) {
            found = &variable;
        }
    }
    return found;
}
}

ConvertSnippetVariablesJob::ConvertSnippetVariablesJob(QObject *parent)
    : QObject(parent)
{
}

ConvertSnippetVariablesJob::~ConvertSnippetVariablesJob() = default;

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
    Q_EMIT textConverted(convertVariables(mComposerViewInterface.get(), mText));
    deleteLater();
}

QString ConvertSnippetVariablesJob::text() const
{
    return mText;
}

MessageComposer::ComposerViewInterface *ConvertSnippetVariablesJob::composerViewInterface() const
{
    return mComposerViewInterface.get();
}

void ConvertSnippetVariablesJob::setComposerViewInterface(MessageComposer::ComposerViewInterface *composerViewInterface)
{
    mComposerViewInterface.reset(composerViewInterface);
}

QString ConvertSnippetVariablesJob::convertVariables(MessageComposer::ComposerViewInterface *composerView, const QString &text)
{
    QString result;
    result.reserve(text.size());
    const qsizetype tmpl_len = text.length();
    for (qsizetype i = 0; i < tmpl_len; ++i) {
        const QChar c = text[i];
        if (c == u'%') {
            const QStringView cmd = QStringView(text).sliced(i + 1);
            if (const SnippetVariable *variable = findSnippetVariable(cmd, composerView != nullptr)) {
                result.append(variable->value(composerView));
                i += variable->name.size();
                continue;
            }
        }
        result.append(c);
    }
    return result;
}

#include "moc_convertsnippetvariablesjob.cpp"

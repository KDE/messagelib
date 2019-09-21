/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "convertsnippetvariablesjob.h"
#include "messagecomposer_debug.h"
#include "composer/composerviewinterface.h"

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
    Q_EMIT textConverted(convertVariables());
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

QString ConvertSnippetVariablesJob::convertVariables() const
{
    QString result;
    const int tmpl_len = mText.length();
    for (int i = 0; i < tmpl_len; ++i) {
        const QChar c = mText[i];
        if (c == QLatin1Char('%')) {
            const QString cmd = mText.mid(i + 1);
            if (cmd.startsWith(QLatin1String("CCADDR"))) {
                i += strlen("CCADDR");
                const QString str = mComposerViewInterface->cc();
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("FULLSUBJECT"))) {
                i += strlen("FULLSUBJECT");
                const QString str = mComposerViewInterface->subject();
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("TOADDR"))) {
                i += strlen("TOADDR");
                const QString str = mComposerViewInterface->to();
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("FROMADDR"))) {
                i += strlen("FROMADDR");
                const QString str = mComposerViewInterface->from();
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("DOW"))) {
                i += strlen("DOW");
                const QString str = mComposerViewInterface->insertDayOfWeek();
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("DATE"))) {
                i += strlen("DATE");
                const QString str = mComposerViewInterface->longDate();
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("SHORTDATE"))) {
                i += strlen("SHORTDATE");
                const QString str = mComposerViewInterface->shortDate();
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("TIME"))) {
                i += strlen("TIME");
                const QString str = mComposerViewInterface->shortTime();
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("TIMELONG"))) {
                i += strlen("TIMELONG");
                const QString str = mComposerViewInterface->longTime();
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("ATTACHMENTCOUNT"))) {
                i += strlen("ATTACHMENTCOUNT");
                const QString str = QString::number(mComposerViewInterface->attachments().count());
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("ATTACHMENTNAMES"))) {
                i += strlen("ATTACHMENTNAMES");
                const QString str = mComposerViewInterface->attachments().names().join(QLatin1Char(','));
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("ATTACHMENTFILENAMES"))) {
                i += strlen("ATTACHMENTFILENAMES");
                const QString str = mComposerViewInterface->attachments().fileNames().join(QLatin1Char(','));
                result.append(str);
            } else if (cmd.startsWith(QLatin1String("ATTACHMENTNAMESANDSIZES"))) {
                i += strlen("ATTACHMENTNAMESANDSIZES");
                const QString str = mComposerViewInterface->attachments().namesAndSize().join(QLatin1Char(','));
                result.append(str);
            } else {
                result.append(c);
            }
        } else {
            result.append(c);
        }
    }
    return result;
}

QString ConvertSnippetVariablesJob::getFirstName(const QString &str)
{
    // simple logic:
    // if there is ',' in name, than format is 'Last, First'
    // else format is 'First Last'
    // last resort -- return 'name' from 'name@domain'
    int sep_pos;
    QString res;
    if ((sep_pos = str.indexOf(QLatin1Char('@'))) > 0) {
        int i;
        for (i = (sep_pos - 1); i >= 0; --i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                res.prepend(c);
            } else {
                break;
            }
        }
    } else if ((sep_pos = str.indexOf(QLatin1Char(','))) > 0) {
        int i;
        bool begin = false;
        const int strLength(str.length());
        for (i = sep_pos; i < strLength; ++i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                begin = true;
                res.append(c);
            } else if (begin) {
                break;
            }
        }
    } else {
        int i;
        const int strLength(str.length());
        for (i = 0; i < strLength; ++i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                res.append(c);
            } else {
                break;
            }
        }
    }
    return res;
}

QString ConvertSnippetVariablesJob::getLastName(const QString &str)
{
    // simple logic:
    // if there is ',' in name, than format is 'Last, First'
    // else format is 'First Last'
    int sep_pos;
    QString res;
    if ((sep_pos = str.indexOf(QLatin1Char(','))) > 0) {
        int i;
        for (i = sep_pos; i >= 0; --i) {
            QChar c = str[i];
            if (c.isLetterOrNumber()) {
                res.prepend(c);
            } else {
                break;
            }
        }
    } else {
        if ((sep_pos = str.indexOf(QLatin1Char(' '))) > 0) {
            bool begin = false;
            const int strLength(str.length());
            for (int i = sep_pos; i < strLength; ++i) {
                QChar c = str[i];
                if (c.isLetterOrNumber()) {
                    begin = true;
                    res.append(c);
                } else if (begin) {
                    break;
                }
            }
        }
    }
    return res;
}

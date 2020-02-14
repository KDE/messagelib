/*
   Copyright (C) 2019-2020 Laurent Montel <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "convertsnippetvariablemenu.h"
#include <KLocalizedString>
#include <QMenu>
#include <QDate>
#include <QTime>
#include <QLocale>
using namespace MessageComposer;
ConvertSnippetVariableMenu::ConvertSnippetVariableMenu(bool onlyMenuForCustomizeAttachmentFileName, QWidget *parentWidget, QObject *parent)
    : QObject(parent)
    , mParentWidget(parentWidget)
{
    initializeMenu(onlyMenuForCustomizeAttachmentFileName);
}

ConvertSnippetVariableMenu::~ConvertSnippetVariableMenu()
{
}

void ConvertSnippetVariableMenu::initializeMenu(bool onlyMenuForCustomizeAttachmentFileName)
{

    QMenu *dateTimeMenuVariable = nullptr;
    if (!onlyMenuForCustomizeAttachmentFileName) {
        mMenu = new QMenu(mParentWidget);
        mMenu->setFocusPolicy(Qt::NoFocus);

        QMenu *toMenuVariable = new QMenu(i18n("To"), mMenu);
        toMenuVariable->addAction(i18n("To Field Address"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::ToAddr);
        });
        toMenuVariable->addAction(i18n("To Field Last Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::ToLname);
        });
        toMenuVariable->addAction(i18n("To Field First Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::ToFname);
        });
        toMenuVariable->addAction(i18n("To Field Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::ToName);
        });
        mMenu->addMenu(toMenuVariable);

        QMenu *fromMenuVariable = new QMenu(i18n("From"), mMenu);
        fromMenuVariable->addAction(i18n("From Field Address"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::FromAddr);
        });
        fromMenuVariable->addAction(i18n("From Field Last Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::FromLname);
        });
        fromMenuVariable->addAction(i18n("From Field First Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::FromFname);
        });
        fromMenuVariable->addAction(i18n("From Field Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::FromName);
        });
        mMenu->addMenu(fromMenuVariable);

        QMenu *ccMenuVariable = new QMenu(i18n("CC"), mMenu);
        ccMenuVariable->addAction(i18n("CC Field Address"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::CcAddr);
        });
        ccMenuVariable->addAction(i18n("CC Field Last Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::CcLname);
        });
        ccMenuVariable->addAction(i18n("CC Field First Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::CcFname);
        });
        ccMenuVariable->addAction(i18n("CC Field Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::CcName);
        });

        mMenu->addMenu(ccMenuVariable);

        QMenu *attachmentMenuVariable = new QMenu(i18n("Attachment"), mMenu);
        attachmentMenuVariable->addAction(i18n("Number Of Attachments"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::AttachmentCount);
        });
        attachmentMenuVariable->addAction(i18n("Names"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::AttachmentName);
        });
        attachmentMenuVariable->addAction(i18n("Filenames"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::AttachmentFilenames);
        });
        attachmentMenuVariable->addAction(i18n("Names and Sizes"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::AttachmentNamesAndSizes);
        });
        mMenu->addMenu(attachmentMenuVariable);
        dateTimeMenuVariable = new QMenu(i18n("Date/Time"), mMenu);
        dateTimeMenuVariable->addAction(i18n("Day Of Week"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::Dow);
        });
        QLocale locale;
        dateTimeMenuVariable->addAction(i18n("Date (%1)", locale.toString(QDate::currentDate(), QLocale::ShortFormat)), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::ShortDate);
        });
        dateTimeMenuVariable->addAction(i18n("Date (%1)", locale.toString(QDate::currentDate(), QLocale::LongFormat)), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::Date);
        });
        dateTimeMenuVariable->addAction(i18n("Time (%1)", locale.toString(QTime::currentTime(), QLocale::ShortFormat)), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::Time);
        });
        dateTimeMenuVariable->addAction(i18n("Time (%1)", locale.toString(QTime::currentTime(), QLocale::LongFormat)), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::TimeLong);
        });
        dateTimeMenuVariable->addAction(i18n("Year"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::Year);
        });
    } else {
        dateTimeMenuVariable = new QMenu(i18n("Date/Time"), mParentWidget);
        mMenu = dateTimeMenuVariable;
    }

    dateTimeMenuVariable->addAction(i18n("Last Year"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::LastYear);
    });
    dateTimeMenuVariable->addAction(i18n("Next Year"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::NextYear);
    });
    dateTimeMenuVariable->addAction(i18n("Month Number"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::MonthNumber);
    });
    dateTimeMenuVariable->addAction(i18n("Day Of Month"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::DayOfMonth);
    });
    dateTimeMenuVariable->addAction(i18n("Week Number"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::WeekNumber);
    });
    dateTimeMenuVariable->addAction(i18n("Month Name Short"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::MonthNameShort);
    });
    dateTimeMenuVariable->addAction(i18n("Month Name Long"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::MonthNameLong);
    });
    dateTimeMenuVariable->addAction(i18n("Day Of Week"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeek);
    });
    dateTimeMenuVariable->addAction(i18n("Day Of Week Name Short"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeekNameShort);
    });
    dateTimeMenuVariable->addAction(i18n("Day Of Week Name Long"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeekNameLong);
    });
    dateTimeMenuVariable->addAction(i18n("Year Last Month"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::YearLastMonth);
    });
    if (!onlyMenuForCustomizeAttachmentFileName) {
        mMenu->addMenu(dateTimeMenuVariable);
        QMenu *miscVariable = new QMenu(i18n("Misc"), mMenu);
        miscVariable->addAction(i18n("Subject"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::FullSubject);
        });
        //Add more variables!
        mMenu->addMenu(miscVariable);
    }
}

QMenu *ConvertSnippetVariableMenu::menu() const
{
    return mMenu;
}

/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "convertsnippetvariablemenu.h"
#include <KLocalizedString>
#include <QMenu>
using namespace MessageComposer;
ConvertSnippetVariableMenu::ConvertSnippetVariableMenu(bool onlyMenuForCustomizeAttachmentFileName, QWidget *parentWidget, QObject *parent)
    : QObject(parent)
    , mParentWidget(parentWidget)
{
    initializeMenu(onlyMenuForCustomizeAttachmentFileName);
}

ConvertSnippetVariableMenu::~ConvertSnippetVariableMenu() = default;

void ConvertSnippetVariableMenu::initializeMenu(bool onlyMenuForCustomizeAttachmentFileName)
{
    QMenu *dateTimeMenuVariable = nullptr;
    if (!onlyMenuForCustomizeAttachmentFileName) {
        mMenu = new QMenu(mParentWidget);
        mMenu->setFocusPolicy(Qt::NoFocus);

        auto toMenuVariable = new QMenu(i18n("To"), mMenu);
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

        auto fromMenuVariable = new QMenu(i18n("From"), mMenu);
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

        auto ccMenuVariable = new QMenu(i18n("CC"), mMenu);
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

        auto bccMenuVariable = new QMenu(i18n("Bcc"), mMenu);
        bccMenuVariable->addAction(i18n("BCC Field Address"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::BccAddr);
        });
        bccMenuVariable->addAction(i18n("BCC Field Last Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::BccLname);
        });
        bccMenuVariable->addAction(i18n("BCC Field First Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::BccFname);
        });
        bccMenuVariable->addAction(i18n("BCC Field Name"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::BccName);
        });

        mMenu->addMenu(bccMenuVariable);

        auto attachmentMenuVariable = new QMenu(i18n("Attachment"), mMenu);
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
        dateTimeMenuVariable->addAction(i18n("Day Of Week (%1)", MessageComposer::ConvertSnippetVariablesUtil::dayOfWeek()), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::Dow);
        });
        dateTimeMenuVariable->addAction(i18n("Date (%1)", MessageComposer::ConvertSnippetVariablesUtil::shortDate()), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::ShortDate);
        });
        dateTimeMenuVariable->addAction(i18n("Long Date (%1)", MessageComposer::ConvertSnippetVariablesUtil::longDate()), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::Date);
        });
        dateTimeMenuVariable->addAction(i18n("Short Time (%1)", MessageComposer::ConvertSnippetVariablesUtil::shortTime()), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::Time);
        });
        dateTimeMenuVariable->addAction(i18n("Time (%1)", MessageComposer::ConvertSnippetVariablesUtil::longTime()), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::TimeLong);
        });
        dateTimeMenuVariable->addAction(i18n("Year (%1)", MessageComposer::ConvertSnippetVariablesUtil::year()), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::Year);
        });
    } else {
        dateTimeMenuVariable = new QMenu(i18n("Date/Time"), mParentWidget);
        mMenu = dateTimeMenuVariable;
    }

    dateTimeMenuVariable->addAction(i18n("Last Year (%1)", MessageComposer::ConvertSnippetVariablesUtil::lastYear()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::LastYear);
    });
    dateTimeMenuVariable->addAction(i18n("Next Year (%1)", MessageComposer::ConvertSnippetVariablesUtil::nextYear()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::NextYear);
    });
    dateTimeMenuVariable->addAction(i18n("Month Number (%1)", MessageComposer::ConvertSnippetVariablesUtil::monthNumber()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::MonthNumber);
    });
    dateTimeMenuVariable->addAction(i18n("Day Of Month (%1)", MessageComposer::ConvertSnippetVariablesUtil::dayOfMonth()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::DayOfMonth);
    });
    dateTimeMenuVariable->addAction(i18n("Week Number (%1)", MessageComposer::ConvertSnippetVariablesUtil::weekNumber()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::WeekNumber);
    });
    dateTimeMenuVariable->addAction(i18n("Month Name Short (%1)", MessageComposer::ConvertSnippetVariablesUtil::monthNameShort()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::MonthNameShort);
    });
    dateTimeMenuVariable->addAction(i18n("Month Name Long (%1)", MessageComposer::ConvertSnippetVariablesUtil::monthNameLong()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::MonthNameLong);
    });
    dateTimeMenuVariable->addAction(i18n("Last Month Name Long (%1)", MessageComposer::ConvertSnippetVariablesUtil::lastMonthNameLong()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::LastMonthNameLong);
    });
    dateTimeMenuVariable->addAction(i18n("Day Of Week (%1)", MessageComposer::ConvertSnippetVariablesUtil::dayOfWeek()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeek);
    });
    dateTimeMenuVariable->addAction(i18n("Day Of Week Name Short (%1)", MessageComposer::ConvertSnippetVariablesUtil::dayOfWeekNameShort()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeekNameShort);
    });
    dateTimeMenuVariable->addAction(i18n("Day Of Week Name Long (%1)", MessageComposer::ConvertSnippetVariablesUtil::dayOfWeekNameLong()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::DayOfWeekNameLong);
    });
    dateTimeMenuVariable->addAction(i18n("Year Last Month (%1)", MessageComposer::ConvertSnippetVariablesUtil::yearLastMonth()), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::YearLastMonth);
    });
    if (!onlyMenuForCustomizeAttachmentFileName) {
        mMenu->addMenu(dateTimeMenuVariable);
        auto miscVariable = new QMenu(i18n("Misc"), mMenu);
        miscVariable->addAction(i18n("Subject"), this, [this]() {
            Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::FullSubject);
        });
        // Add more variables!
        mMenu->addMenu(miscVariable);
    }
}

QMenu *ConvertSnippetVariableMenu::menu() const
{
    return mMenu;
}

#include "moc_convertsnippetvariablemenu.cpp"

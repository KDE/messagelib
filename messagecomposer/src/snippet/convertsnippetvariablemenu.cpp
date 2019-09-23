/*
   Copyright (C) 2019 Montel Laurent <montel@kde.org>

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
ConvertSnippetVariableMenu::ConvertSnippetVariableMenu(QWidget *parentWidget, QObject *parent)
    : QObject(parent)
    , mParentWidget(parentWidget)
{
    initializeMenu();
}

ConvertSnippetVariableMenu::~ConvertSnippetVariableMenu()
{
}

void ConvertSnippetVariableMenu::initializeMenu()
{
    mMenu = new QMenu(mParentWidget);
    mMenu->setFocusPolicy(Qt::NoFocus);

    QMenu *toMenuVariable = new QMenu(i18n("To"), mMenu);
    toMenuVariable->addAction(i18n("Emails"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::ToAddr);
    });
    mMenu->addMenu(toMenuVariable);

    QMenu *fromMenuVariable = new QMenu(i18n("From"), mMenu);
    fromMenuVariable->addAction(i18n("Emails"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::FromAddr);
    });
    mMenu->addMenu(fromMenuVariable);

    QMenu *ccMenuVariable = new QMenu(i18n("Cc"), mMenu);
    ccMenuVariable->addAction(i18n("Emails"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::CcAddr);
    });
    mMenu->addMenu(ccMenuVariable);

    QMenu *attachmentMenuVariable = new QMenu(i18n("Attachment"), mMenu);
    attachmentMenuVariable->addAction(i18n("Number Of Attachments"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::AttachmentCount);
    });
    attachmentMenuVariable->addAction(i18n("Names"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::AttachmentName);
    });
    attachmentMenuVariable->addAction(i18n("Names and Sizes"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::AttachmentNamesAndSizes);
    });
    mMenu->addMenu(attachmentMenuVariable);

    QMenu *dateTimeMenuVariable = new QMenu(i18n("Date/Time"), mMenu);
    dateTimeMenuVariable->addAction(i18n("Day Of Week"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::Dow);
    });
    dateTimeMenuVariable->addAction(i18n("Date (%1)", QDate::currentDate().toString(Qt::SystemLocaleShortDate)), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::ShortDate);
    });
    dateTimeMenuVariable->addAction(i18n("Date (%1)", QDate::currentDate().toString(Qt::SystemLocaleLongDate)), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::Date);
    });
    dateTimeMenuVariable->addAction(i18n("Time (%1)", QTime::currentTime().toString(Qt::SystemLocaleShortDate)), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::Time);
    });
    dateTimeMenuVariable->addAction(i18n("Time (%1)", QTime::currentTime().toString(Qt::SystemLocaleLongDate)), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::TimeLong);
    });
    mMenu->addMenu(dateTimeMenuVariable);

    QMenu *miscVariable = new QMenu(i18n("Misc"), mMenu);
    miscVariable->addAction(i18n("Subject"), this, [this]() {
        Q_EMIT insertVariable(MessageComposer::ConvertSnippetVariablesUtil::FullSubject);
    });
    mMenu->addMenu(miscVariable);
}

QMenu *ConvertSnippetVariableMenu::menu() const
{
    return mMenu;
}

/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateparseremailaddressrequesterlineedit.h"
#include <QHBoxLayout>
#include <QLineEdit>

using namespace TemplateParser;
TemplateParserEmailAddressRequesterLineEdit::TemplateParserEmailAddressRequesterLineEdit(QWidget *parent)
    : TemplateParser::TemplateParserEmailAddressRequesterBase(parent)
    , mLineEdit(new QLineEdit(this))
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainlayout"));
    mainLayout->setContentsMargins({});
    mLineEdit->setObjectName(QLatin1StringView("lineedit"));
    mainLayout->addWidget(mLineEdit);
    connect(mLineEdit, &QLineEdit::textChanged, this, &TemplateParserEmailAddressRequesterLineEdit::textChanged);
}

TemplateParserEmailAddressRequesterLineEdit::~TemplateParserEmailAddressRequesterLineEdit()
{
    disconnect(mLineEdit, &QLineEdit::textChanged, this, &TemplateParserEmailAddressRequesterLineEdit::textChanged);
}

QString TemplateParserEmailAddressRequesterLineEdit::text() const
{
    return mLineEdit->text();
}

void TemplateParserEmailAddressRequesterLineEdit::setText(const QString &str)
{
    mLineEdit->setText(str);
}

void TemplateParserEmailAddressRequesterLineEdit::clear()
{
    mLineEdit->clear();
}

#include "moc_templateparseremailaddressrequesterlineedit.cpp"

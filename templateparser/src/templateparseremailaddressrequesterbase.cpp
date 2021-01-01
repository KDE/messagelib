/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateparseremailaddressrequesterbase.h"

using namespace TemplateParser;
TemplateParserEmailAddressRequesterBase::TemplateParserEmailAddressRequesterBase(QWidget *parent)
    : QWidget(parent)
{
}

TemplateParserEmailAddressRequesterBase::~TemplateParserEmailAddressRequesterBase()
{
}

QString TemplateParserEmailAddressRequesterBase::text() const
{
    return {};
}

void TemplateParserEmailAddressRequesterBase::setText(const QString &str)
{
    Q_UNUSED(str)
}

void TemplateParserEmailAddressRequesterBase::clear()
{
}

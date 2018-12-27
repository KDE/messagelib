/*
   Copyright (C) 2017-2019 Laurent Montel <montel@kde.org>

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

#include "templateparseremailaddressrequesterinterfacewidget.h"
#include <QHBoxLayout>
#include <TemplateParser/TemplateParserEmailAddressRequesterBase>
#include "templateparseremailaddressrequesterlineedit.h"
#include <KPluginLoader>
#include <KPluginFactory>

using namespace TemplateParser;

TemplateParserEmailAddressRequesterInterfaceWidget::TemplateParserEmailAddressRequesterInterfaceWidget(QWidget *parent)
    : QWidget(parent)
    , mTemplateParserEmailBase(nullptr)
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mainLayout->setMargin(0);
    initializeEmailWidget();
    mainLayout->addWidget(mTemplateParserEmailBase);
}

void TemplateParserEmailAddressRequesterInterfaceWidget::initializeEmailWidget()
{
    KPluginLoader loader(QStringLiteral("templateparser/templateparseraddressrequesterplugin"));
    KPluginFactory *factory = loader.factory();
    if (factory) {
        mTemplateParserEmailBase = factory->create<TemplateParser::TemplateParserEmailAddressRequesterBase>(this);
    } else {
        mTemplateParserEmailBase = new TemplateParser::TemplateParserEmailAddressRequesterLineEdit(this);
    }
    mTemplateParserEmailBase->setObjectName(QStringLiteral("templateparseremailbase"));
    connect(mTemplateParserEmailBase, &TemplateParserEmailAddressRequesterBase::textChanged,
            this, &TemplateParserEmailAddressRequesterInterfaceWidget::textChanged);
}

QString TemplateParserEmailAddressRequesterInterfaceWidget::text() const
{
    return mTemplateParserEmailBase->text();
}

void TemplateParserEmailAddressRequesterInterfaceWidget::setText(const QString &str)
{
    mTemplateParserEmailBase->setText(str);
}

void TemplateParserEmailAddressRequesterInterfaceWidget::clear()
{
    mTemplateParserEmailBase->clear();
}

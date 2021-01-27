/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateparseremailaddressrequesterinterfacewidget.h"
#include "templateparseremailaddressrequesterlineedit.h"
#include <KPluginFactory>
#include <KPluginLoader>
#include <QHBoxLayout>
#include <TemplateParser/TemplateParserEmailAddressRequesterBase>

using namespace TemplateParser;

TemplateParserEmailAddressRequesterInterfaceWidget::TemplateParserEmailAddressRequesterInterfaceWidget(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QStringLiteral("mainlayout"));
    mainLayout->setContentsMargins({});
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
    connect(mTemplateParserEmailBase,
            &TemplateParserEmailAddressRequesterBase::textChanged,
            this,
            &TemplateParserEmailAddressRequesterInterfaceWidget::textChanged);
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

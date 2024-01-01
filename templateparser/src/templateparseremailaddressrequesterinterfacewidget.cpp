/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateparseremailaddressrequesterinterfacewidget.h"
#include "templateparseremailaddressrequesterlineedit.h"
#include <KPluginFactory>
#include <QHBoxLayout>
#include <TemplateParser/TemplateParserEmailAddressRequesterBase>

using namespace TemplateParser;

TemplateParserEmailAddressRequesterInterfaceWidget::TemplateParserEmailAddressRequesterInterfaceWidget(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QHBoxLayout(this);
    mainLayout->setObjectName(QLatin1StringView("mainlayout"));
    mainLayout->setContentsMargins({});
    initializeEmailWidget();
    mainLayout->addWidget(mTemplateParserEmailBase);
}

void TemplateParserEmailAddressRequesterInterfaceWidget::initializeEmailWidget()
{
    const KPluginMetaData editWidgetPlugin(QStringLiteral("pim6/templateparser/templateparseraddressrequesterplugin"));

    const auto result = KPluginFactory::instantiatePlugin<TemplateParser::TemplateParserEmailAddressRequesterBase>(editWidgetPlugin, this);
    if (result) {
        mTemplateParserEmailBase = result.plugin;
    } else {
        mTemplateParserEmailBase = new TemplateParser::TemplateParserEmailAddressRequesterLineEdit(this);
    }
    mTemplateParserEmailBase->setObjectName(QLatin1StringView("templateparseremailbase"));
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

#include "moc_templateparseremailaddressrequesterinterfacewidget.cpp"

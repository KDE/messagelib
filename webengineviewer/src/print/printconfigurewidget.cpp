/*
  Copyright (c) 2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "printconfigurewidget.h"
#include <QHBoxLayout>
#include <KLocalizedString>
#include <QLabel>
#include <QToolButton>

using namespace WebEngineViewer;

PrintConfigureWidget::PrintConfigureWidget(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    setLayout(layout);

    QVBoxLayout *printLayout = new QVBoxLayout;
    printLayout->setObjectName(QStringLiteral("printLayout"));
    layout->addLayout(printLayout);
    printLayout->setMargin(0);

    mPrintLayoutLabel = new QLabel(this);
    mPrintLayoutLabel->setObjectName(QStringLiteral("printlayoutlabel"));
    layout->addWidget(mPrintLayoutLabel);

    mSelectPrintLayout = new QToolButton(this);
    mSelectPrintLayout->setText(i18n("..."));
    mSelectPrintLayout->setObjectName(QStringLiteral("selectprintlayout"));
    mSelectPrintLayout->setToolTip(i18n("Select Print Layout"));
    layout->addWidget(mSelectPrintLayout);
    connect(mSelectPrintLayout, &QToolButton::clicked, this, &PrintConfigureWidget::slotSelectPrintLayout);

}

PrintConfigureWidget::~PrintConfigureWidget()
{

}

void PrintConfigureWidget::slotSelectPrintLayout()
{
    //TODO
}

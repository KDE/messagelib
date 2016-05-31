/*
   Copyright (C) 2016 Laurent Montel <montel@kde.org>

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

#include "printconfigurewidget.h"
#include "webengineviewer_debug.h"
#include <QHBoxLayout>
#include <KLocalizedString>
#include <QLabel>
#include <QToolButton>
#include <QPageSetupDialog>
#include <QPrinter>

using namespace WebEngineViewer;

PrintConfigureWidget::PrintConfigureWidget(QWidget *parent)
    : QWidget(parent),
      mCurrentPageLayout(QPageLayout(QPageSize(QPageSize::A4), QPageLayout::Portrait, QMarginsF(0.0, 0.0, 0.0, 0.0)))
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

    updatePageLayoutLabel();
}

PrintConfigureWidget::~PrintConfigureWidget()
{

}

void PrintConfigureWidget::slotSelectPrintLayout()
{
    QPrinter printer;

    if (!printer.setPageLayout(mCurrentPageLayout)) {
        qCDebug(WEBENGINEVIEWER_LOG) << "Print Setup unsupported";
    }
    QPageSetupDialog dlg(&printer, this);
    if (dlg.exec() != QDialog::Accepted) {
        return;
    }
    mCurrentPageLayout = printer.pageLayout();
    updatePageLayoutLabel();
}

QPageLayout PrintConfigureWidget::currentPageLayout() const
{
    return mCurrentPageLayout;
}

void PrintConfigureWidget::updatePageLayoutLabel()
{
    mPrintLayoutLabel->setText(i18n("Print Layout: %1, %2",
                                    mCurrentPageLayout.pageSize().name(),
                                    mCurrentPageLayout.orientation() == QPageLayout::Portrait
                                    ? i18n("Portrait") : i18n("Landscape")));
}

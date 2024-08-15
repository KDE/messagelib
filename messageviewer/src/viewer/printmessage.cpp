/*
  SPDX-FileCopyrightText: 2023-2024 Laurent Montel <montel.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "printmessage.h"

#include <QPainter>
#include <QPrintDialog>
#include <QPrintPreviewDialog>
#include <QWebEngineView>

#include <KLocalizedString>

using namespace MessageViewer;
PrintMessage::PrintMessage(QObject *parent)
    : QObject{parent}
{
}

PrintMessage::~PrintMessage() = default;

void PrintMessage::setView(QWebEngineView *view)
{
    Q_ASSERT(!mView);
    mView = view;
    connect(mView, &QWebEngineView::printRequested, this, &PrintMessage::printPreview);
    connect(mView, &QWebEngineView::printFinished, this, &PrintMessage::printFinished);
}

void PrintMessage::setDocumentName(const QString &name)
{
    mDocumentName = name;
}

void PrintMessage::print()
{
    QPrintDialog dialog(&mPrinter, mView);
    if (dialog.exec() != QDialog::Accepted) {
        deleteLater();
        return;
    }
    printDocument(&mPrinter, dialog.printer()->pageLayout());
}

void PrintMessage::printPreviewDocument(QPrinter *printer)
{
    mView->print(printer);
    mWaitForResult.exec();
}

void PrintMessage::printDocument(QPrinter *printer, const QPageLayout &layout)
{
    printer->setPageLayout(layout);
    mView->print(printer);
    mWaitForResult.exec();
}

void PrintMessage::printFinished(bool success)
{
    if (!success) {
        QPainter painter;
        if (painter.begin(&mPrinter)) {
            QFont font = painter.font();
            font.setPixelSize(20);
            painter.setFont(font);
            painter.drawText(QPointF(10, 25), i18n("Could not generate print preview."));
            painter.end();
        }
    }
    mWaitForResult.quit();
    Q_EMIT printingFinished();
}

QWidget *PrintMessage::parentWidget() const
{
    return mParentWidget;
}

void PrintMessage::setParentWidget(QWidget *newParentWidget)
{
    mParentWidget = newParentWidget;
}

QString PrintMessage::documentName() const
{
    return mDocumentName;
}

void PrintMessage::printPreview()
{
    if (!mView) {
        deleteLater();
        return;
    }
    if (mInPrintPreview) {
        deleteLater();
        return;
    }
    mInPrintPreview = true;

    QPrintPreviewDialog preview(&mPrinter, mParentWidget);
    preview.setWindowTitle(i18nc("@title:window", "Print Document"));
    preview.resize(800, 750);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, &PrintMessage::printPreviewDocument);
    preview.exec();
    mInPrintPreview = false;
    deleteLater();
}

#include "moc_printmessage.cpp"

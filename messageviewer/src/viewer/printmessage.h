/*
  SPDX-FileCopyrightText: 2023-2026 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <QEventLoop>
#include <QObject>
#include <QPrinter>

class QWebEngineView;
namespace MessageViewer
{
/*!
 * \class MessageViewer::PrintMessage
 * \inmodule MessageViewer
 * \inheaderfile MessageViewer/PrintMessage
 * \brief The PrintMessage class
 * \author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT PrintMessage : public QObject
{
    Q_OBJECT
public:
    /*! Constructs a PrintMessage object with the given parent. */
    explicit PrintMessage(QObject *parent = nullptr);
    /*! Destroys the PrintMessage object. */
    ~PrintMessage() override;

    /*! Sets the web engine view to print. */
    void setView(QWebEngineView *view);
    /*! Sets the document name for printing. */
    void setDocumentName(const QString &name);

    /*! Prints the document. */
    void print();
    /*! Shows a print preview of the document. */
    void printPreview();

    /*! Returns the document name. */
    [[nodiscard]] QString documentName() const;

    /*! Returns the parent widget for the print dialog. */
    [[nodiscard]] QWidget *parentWidget() const;
    /*! Sets the parent widget for the print dialog. */
    void setParentWidget(QWidget *newParentWidget);

Q_SIGNALS:
    /*! Emitted when printing has finished. */
    void printingFinished();

private:
    MESSAGEVIEWER_NO_EXPORT void printDocument(QPrinter *printer, const QPageLayout &layout);
    MESSAGEVIEWER_NO_EXPORT void printFinished(bool success);
    MESSAGEVIEWER_NO_EXPORT void printPreviewDocument(QPrinter *printer);
    QString mDocumentName;
    QPrinter mPrinter;
    QEventLoop mWaitForResult;
    QWidget *mParentWidget = nullptr;
    QWebEngineView *mView = nullptr;
    bool mInPrintPreview = false;
};
}

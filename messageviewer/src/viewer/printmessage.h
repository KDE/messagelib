/*
  SPDX-FileCopyrightText: 2023 Laurent Montel <montel@kde.org>

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
class MESSAGEVIEWER_EXPORT PrintMessage : public QObject
{
    Q_OBJECT
public:
    explicit PrintMessage(QObject *parent = nullptr);
    ~PrintMessage() override;

    void setView(QWebEngineView *view);
    void setDocumentName(const QString &name);

    void print();
    void printPreview();

    [[nodiscard]] QString documentName() const;

    [[nodiscard]] QWidget *parentWidget() const;
    void setParentWidget(QWidget *newParentWidget);

Q_SIGNALS:
    void printingFinished();

private:
    void printDocument(QPrinter *printer);
    void printFinished(bool success);
    QString mDocumentName;
    QPrinter mPrinter;
    QEventLoop mWaitForResult;
    QWidget *mParentWidget = nullptr;
    QWebEngineView *mView = nullptr;
    bool mInPrintPreview = false;
};
}

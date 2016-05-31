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

#ifndef PRINTPREVIEWPAGEWIDGET_H
#define PRINTPREVIEWPAGEWIDGET_H

#include <QWidget>

class QComboBox;
class QLabel;
namespace Poppler
{
class Document;
}
namespace WebEngineViewer
{
class PrintPreviewPageViewer;
class PrintPreviewPageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PrintPreviewPageWidget(QWidget *parent = Q_NULLPTR);
    ~PrintPreviewPageWidget();

    void loadFile(const QString &path, bool deleteFile = false);

    void print(const QList<int> &page);
    bool deleteFile() const;

private Q_SLOTS:
    void showPage(int index);

private:
    void fillComboBox();
    QString mFilePath;
    bool mDeleteFile;
    PrintPreviewPageViewer *mPrintPreviewPage;
    Poppler::Document *mDoc;
    QComboBox *mPageComboBox;
    QLabel *mPageComboboxLab;
};
}
#endif // PRINTPREVIEWPAGEWIDGET_H

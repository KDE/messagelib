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

#ifndef PRINTPREVIEWPAGEWIDGET_H
#define PRINTPREVIEWPAGEWIDGET_H

#include <QWidget>

class QComboBox;
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

    void loadFile(const QString &path);

    void print(const QList<int> &page);
private Q_SLOTS:
    void showPage(int index);

private:
    void fillComboBox();
    PrintPreviewPageViewer *mPrintPreviewPage;
    Poppler::Document *mDoc;
    QComboBox *mPageComboBox;
};
}
#endif // PRINTPREVIEWPAGEWIDGET_H

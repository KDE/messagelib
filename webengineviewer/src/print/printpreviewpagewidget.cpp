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

#include "printpreviewpagewidget.h"
#include "printpreviewpageviewer.h"
#include "webengineviewer_debug.h"

#include <poppler-qt5.h>

#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QFile>
#include <QVBoxLayout>
#include <KMessageBox>
#include <KLocalizedString>

using namespace WebEngineViewer;

PrintPreviewPageWidget::PrintPreviewPageWidget(QWidget *parent)
    : QWidget(parent),
      mDeleteFile(false),
      mDoc(Q_NULLPTR)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setObjectName(QStringLiteral("layoutcombobox"));
    hbox->setMargin(0);
    layout->addLayout(hbox);
    mPageComboboxLab = new QLabel(i18n("Page:"), this);
    mPageComboboxLab->setObjectName(QStringLiteral("labelcombobox"));
    hbox->addWidget(mPageComboboxLab);

    mPageComboBox = new QComboBox(this);
    mPageComboBox->setObjectName(QStringLiteral("pagecombobox"));
    hbox->addWidget(mPageComboBox);
    hbox->addStretch(1);
    connect(mPageComboBox, SIGNAL(activated(int)), this, SLOT(showPage(int)));

    mPrintPreviewPage = new PrintPreviewPageViewer(this);
    mPrintPreviewPage->setObjectName(QStringLiteral("printpreviewpage"));
    layout->addWidget(mPrintPreviewPage);

    setLayout(layout);
}

PrintPreviewPageWidget::~PrintPreviewPageWidget()
{
    if (mDeleteFile && !mFilePath.isEmpty()) {
        QFile file(mFilePath);
        bool removeFile = file.remove();
        if (removeFile) {
            qCWarning(WEBENGINEVIEWER_LOG) << "File removed " << mFilePath;
        } else {
            qCWarning(WEBENGINEVIEWER_LOG) << "Impossible to remove file " << mFilePath;
        }
    }
    delete mDoc;
    mDoc = Q_NULLPTR;
}

void PrintPreviewPageWidget::loadFile(const QString &path, bool deleteFile)
{
    if (path.isEmpty()) {
        return;
    }
    mDeleteFile = deleteFile;
    mFilePath = path;
    mDoc = Poppler::Document::load(path);
    if (!mDoc) {
        KMessageBox::error(this, i18n("Unable to open file \"%1\"", path), i18n("Open file error"));
        return;
    }
    mDoc->setRenderHint(Poppler::Document::TextAntialiasing, true);
    mDoc->setRenderHint(Poppler::Document::Antialiasing, true);

    fillComboBox();

    showPage(0);
}

void PrintPreviewPageWidget::fillComboBox()
{
    mPageComboBox->clear();
    const int pageCount = mDoc->numPages();
    for (int i = 0; i < pageCount; ++i) {
        mPageComboBox->addItem(QString::number(i + 1));
    }
    if (pageCount == 1) {
        mPageComboBox->hide();
        mPageComboboxLab->hide();
    }
}

bool PrintPreviewPageWidget::deleteFile() const
{
    return mDeleteFile;
}

void PrintPreviewPageWidget::showPage(int index)
{
    if (index >= 0) {
        Poppler::Page *popplerPage = mDoc->page(index);
        mPrintPreviewPage->showPage(popplerPage);
        delete popplerPage;
    }
}

void PrintPreviewPageWidget::print(const QList<int> &page)
{
    //TODO
}

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

#include "printpreviewpagewidget.h"
#include "printpreviewpageviewer.h"
#include "webengineviewer_debug.h"

#include <poppler-qt5.h>

#include <QLabel>
#include <QDir>
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
      mDoc(Q_NULLPTR),
      mPageCount(0)
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
    connect(mPageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(showPage(int)));

    mPrintPreviewPage = new PrintPreviewPageViewer(this);
    mPrintPreviewPage->setObjectName(QStringLiteral("printpreviewpage"));
    layout->addWidget(mPrintPreviewPage);
    connect(mPrintPreviewPage, &PrintPreviewPageViewer::reloadPage, this, &PrintPreviewPageWidget::slotReloadPage);
    connect(mPrintPreviewPage, &PrintPreviewPageViewer::pageDown, this, &PrintPreviewPageWidget::slotPageDown);
    connect(mPrintPreviewPage, &PrintPreviewPageViewer::pageUp, this, &PrintPreviewPageWidget::slotPageUp);

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

bool PrintPreviewPageWidget::loadFile(const QString &path, bool deleteFile)
{
    if (path.isEmpty()) {
        return false;
    }
    mDeleteFile = deleteFile;
    mFilePath = path;
    mDoc = Poppler::Document::load(path);
    if (!mDoc) {
        KMessageBox::error(this, i18n("Unable to open file \"%1\"", path), i18n("Open file error"));
        return false;
    }
    mDoc->setRenderHint(Poppler::Document::TextAntialiasing, true);
    mDoc->setRenderHint(Poppler::Document::Antialiasing, true);

    fillComboBox();

    showPage(0);
    return true;
}

void PrintPreviewPageWidget::fillComboBox()
{
    mPageComboBox->clear();
    mPageCount = mDoc->numPages();
    if (mPageCount == 1) {
        mPageComboBox->hide();
        mPageComboboxLab->hide();
    } else {
        for (int i = 0; i < mPageCount; ++i) {
            mPageComboBox->addItem(QString::number(i + 1));
        }
    }
}

int PrintPreviewPageWidget::pageCount() const
{
    return mPageCount;
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

void PrintPreviewPageWidget::print(const QList<int> &pages)
{
#if 0
    Poppler::PSConverter *psConverter = mDoc->psConverter();
    QTemporaryFile tf(QDir::tempPath() + QLatin1String("/kmail_XXXXXX.ps"));
    if (!tf.open()) {

    }
    psConverter->setPageList(pages);
    psConverter->setRightMargin(0);
    psConverter->setBottomMargin(0);
    psConverter->setLeftMargin(0);
    psConverter->setTopMargin(0);
    psConverter->setStrictMargins(false);
    psConverter->setPSOptions(psConverter->psOptions() | Poppler::PSConverter::HideAnnotations);
    if (psConverter->convert()) {
        if (!QStandardPaths::findExecutable(QStringLiteral("lpr-cups")).isEmpty()) {
            exe = QStringLiteral("lpr-cups");
        } else if (!QStandardPaths::findExecutable(QStringLiteral("lpr.cups")).isEmpty()) {
            exe = QStringLiteral("lpr.cups");
        } else if (!QStandardPaths::findExecutable(QStringLiteral("lpr")).isEmpty()) {
            exe = QStringLiteral("lpr");
        } else if (!QStandardPaths::findExecutable(QStringLiteral("lp")).isEmpty()) {
            exe = QStringLiteral("lp");
        } else {

        }
        delete psConverter;
#else
    qDebug() << " void PrintPreviewPageWidget::print(const QList<int> &page) not implemented";
#endif
        //TODO
    }

    void PrintPreviewPageWidget::slotReloadPage()
    {
        const int currentIndex = mPageComboBox->currentIndex();
        showPage(currentIndex == -1 ? 0 : currentIndex);
    }

    void PrintPreviewPageWidget::slotPageDown()
    {
        const int currentIndex = mPageComboBox->currentIndex();
        if (currentIndex != -1) {
            const int newIndex = currentIndex + 1;
            if (newIndex < mPageComboBox->count()) {
                mPageComboBox->setCurrentIndex(newIndex);
            }
        }
    }

    void PrintPreviewPageWidget::slotPageUp()
    {
        const int currentIndex = mPageComboBox->currentIndex();
        if (currentIndex != -1) {
            const int newIndex = currentIndex - 1;
            if (newIndex >= 0) {
                mPageComboBox->setCurrentIndex(newIndex);
            }
        }
    }

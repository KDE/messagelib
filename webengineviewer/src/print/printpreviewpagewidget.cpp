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

#include <poppler-qt5.h>

#include <QLabel>
#include <QComboBox>
#include <QVBoxLayout>
#include <KMessageBox>
#include <KLocalizedString>

using namespace WebEngineViewer;

PrintPreviewPageWidget::PrintPreviewPageWidget(QWidget *parent)
    : QWidget(parent),
      mDoc(Q_NULLPTR)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->setObjectName(QStringLiteral("layoutcombobox"));
    hbox->setMargin(0);
    layout->addLayout(hbox);
    QLabel *lab = new QLabel(i18n("Page:"), this);
    lab->setObjectName(QStringLiteral("labelcombobox"));
    hbox->addWidget(lab);

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
    delete mDoc;
    mDoc = Q_NULLPTR;
}

void PrintPreviewPageWidget::loadFile(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }
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
}

void PrintPreviewPageWidget::showPage(int index)
{
    Poppler::Page *popplerPage = mDoc->page(index);
    mPrintPreviewPage->showPage(popplerPage);
    delete popplerPage;
}

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

#include "printpreviewdialog.h"
#include "printpreviewpageviewer.h"
#include <poppler-qt5.h>

#include <KLocalizedString>
#include <KMessageBox>
#include <QVBoxLayout>
#include <QDialogButtonBox>

using namespace WebEngineViewer;

PrintPreviewDialog::PrintPreviewDialog(QWidget *parent)
    : QDialog(parent),
      mDoc(Q_NULLPTR)
{
    QVBoxLayout *layout = new QVBoxLayout;
    setLayout(layout);

    mPrintPreviewPage = new PrintPreviewPageViewer(this);
    mPrintPreviewPage->setObjectName(QStringLiteral("printpreviewpage"));
    layout->addWidget(mPrintPreviewPage);
}

PrintPreviewDialog::~PrintPreviewDialog()
{

}

void PrintPreviewDialog::loadFile(const QString &path)
{
    if (path.isEmpty()) {
        return;
    }
    mDoc = Poppler::Document::load(path);
    if (!mDoc) {
        KMessageBox::error(this, i18n("Unable to open file \"%1\"", path), i18n("Open file error"));
        return;
    }

    //TODO
}

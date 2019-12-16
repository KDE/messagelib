/*
  Copyright (c) 2013-2019 Montel Laurent <montel@kde.org>

  This library is free software; you can redistribute it and/or modify it
  under the terms of the GNU Library General Public License as published by
  the Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
  License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

*/

#include "scamdetectiondetailsdialog.h"
#include "messageviewer_debug.h"
#include "settings/messageviewersettings.h"

#include "kpimtextedit/richtexteditorwidget.h"

#include <KLocalizedString>

#include <QUrl>

#include <KStandardGuiItem>
#include <QFileDialog>

#include <QTextStream>
#include <QDialogButtonBox>
#include <KConfigGroup>
#include <QPushButton>
#include <KGuiItem>
#include <QVBoxLayout>
#include <memory>

using namespace MessageViewer;

ScamDetectionDetailsDialog::ScamDetectionDetailsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(i18nc("@title:window", "Details"));
    setAttribute(Qt::WA_DeleteOnClose);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    QPushButton *user1Button = new QPushButton(this);
    buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ScamDetectionDetailsDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ScamDetectionDetailsDialog::reject);
    KGuiItem::assign(user1Button, KStandardGuiItem::saveAs());
    setModal(false);
    mDetails = new KPIMTextEdit::RichTextEditorWidget(this);
    mainLayout->addWidget(mDetails);
    mainLayout->addWidget(buttonBox);
    mDetails->setReadOnly(true);
    connect(user1Button, &QPushButton::clicked, this, &ScamDetectionDetailsDialog::slotSaveAs);
    readConfig();
}

ScamDetectionDetailsDialog::~ScamDetectionDetailsDialog()
{
    writeConfig();
}

void ScamDetectionDetailsDialog::slotSaveAs()
{
    QUrl url;
    std::unique_ptr<QFileDialog> fdlg(new QFileDialog(this, QString(), url.path()));
    fdlg->setAcceptMode(QFileDialog::AcceptSave);
    fdlg->setFileMode(QFileDialog::AnyFile);
    fdlg->selectFile(QStringLiteral("scam-detection.html"));
    if (fdlg->exec() == QDialog::Accepted) {
        const QStringList fileNames = fdlg->selectedFiles();
        if (!fileNames.isEmpty()) {
            QFile file(fileNames.at(0));
            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qCDebug(MESSAGEVIEWER_LOG) << "We can't save in file :" << fileNames.at(0);
                return;
            }
            QTextStream ts(&file);
            ts.setCodec("UTF-8");
            QString htmlStr = mDetails->toHtml();
            htmlStr.replace(QLatin1String("meta name=\"qrichtext\" content=\"1\""),
                            QLatin1String(
                                "meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\""));
            ts <<  htmlStr;
            file.close();
        }
    }
}

void ScamDetectionDetailsDialog::setDetails(const QString &details)
{
    mDetails->setHtml(details);
}

void ScamDetectionDetailsDialog::readConfig()
{
    KConfigGroup group(
        KSharedConfig::openConfig(), "ScamDetectionDetailsDialog");
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void ScamDetectionDetailsDialog::writeConfig()
{
    KConfigGroup group(
        KSharedConfig::openConfig(), "ScamDetectionDetailsDialog");
    group.writeEntry("Size", size());
    group.sync();
}

/*
  SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later

*/

#include "scamdetectiondetailsdialog.h"
#include "messageviewer_debug.h"
#include "settings/messageviewersettings.h"

#include <KPIMTextEdit/RichTextEditorWidget>

#include <KLocalizedString>

#include <QUrl>

#include <KStandardGuiItem>
#include <QFileDialog>

#include <KConfigGroup>
#include <KGuiItem>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QTextStream>
#include <QVBoxLayout>
#include <memory>

using namespace MessageViewer;
namespace
{
static const char myScamDetectionDetailsDialogConfigGroupName[] = "ScamDetectionDetailsDialog";
}
ScamDetectionDetailsDialog::ScamDetectionDetailsDialog(QWidget *parent)
    : QDialog(parent)
    , mDetails(new KPIMTextEdit::RichTextEditorWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Details"));
    setAttribute(Qt::WA_DeleteOnClose);
    auto mainLayout = new QVBoxLayout(this);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    auto user1Button = new QPushButton(this);
    buttonBox->addButton(user1Button, QDialogButtonBox::ActionRole);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ScamDetectionDetailsDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ScamDetectionDetailsDialog::reject);
    KGuiItem::assign(user1Button, KStandardGuiItem::saveAs());
    setModal(false);
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            ts.setCodec("UTF-8");
#endif
            QString htmlStr = mDetails->toHtml();
            htmlStr.replace(QLatin1String(R"(meta name="qrichtext" content="1")"),
                            QLatin1String(R"(meta http-equiv="Content-Type" content="text/html; charset=UTF-8")"));
            ts << htmlStr;
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
    KConfigGroup group(KSharedConfig::openStateConfig(), myScamDetectionDetailsDialogConfigGroupName);
    const QSize size = group.readEntry("Size", QSize(600, 400));
    if (size.isValid()) {
        resize(size);
    }
}

void ScamDetectionDetailsDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), myScamDetectionDetailsDialogConfigGroupName);
    group.writeEntry("Size", size());
    group.sync();
}

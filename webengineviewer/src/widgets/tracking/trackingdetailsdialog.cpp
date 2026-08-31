/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "trackingdetailsdialog.h"
#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KWindowConfig>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWindow>
#include <TextAddonsWidgets/LoadDialogSizeUtils>
#include <TextCustomEditor/RichTextEditorWidget>

using namespace Qt::Literals::StringLiterals;

using namespace WebEngineViewer;
namespace
{
static const char myMailTrackingDetailsDialogConfigGroupName[] = "MailTrackingDetailsDialog";
}
TrackingDetailsDialog::TrackingDetailsDialog(QWidget *parent)
    : QDialog(parent)
    , mDetails(new TextCustomEditor::RichTextEditorWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Details"));
    setAttribute(Qt::WA_DeleteOnClose);
    setModal(false);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setObjectName("mainLayout"_L1);

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Close, this);
    buttonBox->setObjectName("buttonbox"_L1);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &TrackingDetailsDialog::reject);
    connect(buttonBox->button(QDialogButtonBox::Close), &QPushButton::clicked, this, &TrackingDetailsDialog::close);

    mDetails->setObjectName("detail"_L1);
    mainLayout->addWidget(mDetails);
    mainLayout->addWidget(buttonBox);
    mDetails->setReadOnly(true);
    readConfig();
}

TrackingDetailsDialog::~TrackingDetailsDialog()
{
    writeConfig();
}

void TrackingDetailsDialog::readConfig()
{
    create(); // ensure a window is created
    TextAddonsWidgets::LoadDialogSizeUtils::loadDialogSizeScaled(this, QLatin1StringView(myMailTrackingDetailsDialogConfigGroupName), 600, 400);
}

void TrackingDetailsDialog::writeConfig()
{
    KConfigGroup group(KSharedConfig::openStateConfig(), QLatin1StringView(myMailTrackingDetailsDialogConfigGroupName));
    KWindowConfig::saveWindowSize(windowHandle(), group);
}

void TrackingDetailsDialog::setDetails(const QString &details)
{
    mDetails->setHtml(details);
}

#include "moc_trackingdetailsdialog.cpp"

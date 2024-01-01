/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sendlaterdialog.h"
#include "sendlaterinfo.h"
#include "sendlatertimedatewidget_p.h"
#include "ui_sendlaterwidget.h"

#include <KLocalizedString>
#include <KSeparator>
#include <QIcon>

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

using namespace MessageComposer;

SendLaterDialog::SendLaterDialog(SendLaterInfo *info, QWidget *parent)
    : QDialog(parent)
    , mSendLaterWidget(new Ui::SendLaterWidget)
    , mInfo(info)
{
    setWindowTitle(i18nc("@title:window", "Send Later"));
    setWindowIcon(QIcon::fromTheme(QStringLiteral("kmail")));

    auto mainLayout = new QVBoxLayout(this);

    auto sendLaterWidget = new QWidget(this);
    mSendLaterWidget->setupUi(sendLaterWidget);

    auto w = new QWidget(this);
    auto lay = new QVBoxLayout(w);
    lay->setContentsMargins({});

    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    mOkButton = buttonBox->button(QDialogButtonBox::Ok);
    mOkButton->setObjectName(QLatin1StringView("okbutton"));
    mOkButton->setDefault(true);
    mOkButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SendLaterDialog::reject);

    if (!info) {
        mOkButton->setText(i18n("Send Later"));
        mDelay = new QCheckBox(i18n("Delay"));
        mDelay->setChecked(false);
        slotDelay(false);
        connect(mDelay, &QCheckBox::clicked, this, &SendLaterDialog::slotDelay);
        lay->addWidget(mDelay);
    }

    connect(mOkButton, &QPushButton::clicked, this, &SendLaterDialog::slotOkClicked);

    lay->addWidget(sendLaterWidget);

    QDateTime t = QDateTime::currentDateTime();
    t = t.addSecs(60 * 60);

    mSendLaterWidget->mDateTime->setDateTime(t);
    connect(mSendLaterWidget->mRecurrence, &QCheckBox::clicked, this, &SendLaterDialog::slotRecurrenceClicked);
    const QStringList unitsList = {i18n("Days"), i18n("Weeks"), i18n("Months"), i18n("Years")};
    mSendLaterWidget->mRecurrenceComboBox->addItems(unitsList);
    connect(mSendLaterWidget->mDateTime, &SendLaterTimeDateWidget::dateChanged, this, &SendLaterDialog::slotDateChanged);

    lay->addWidget(new KSeparator);

    mainLayout->addWidget(w);
    mainLayout->addWidget(buttonBox);
    slotRecurrenceClicked(false);
    if (info) {
        load(info);
    }
    resize(180, 120);
}

SendLaterDialog::~SendLaterDialog()
{
    delete mSendLaterWidget;
}

void SendLaterDialog::slotDateChanged(const QString &date)
{
    mOkButton->setEnabled(!date.isEmpty());
}

void SendLaterDialog::slotRecurrenceClicked(bool clicked)
{
    mSendLaterWidget->mRecurrenceValue->setEnabled(clicked);
    mSendLaterWidget->mRecurrenceComboBox->setEnabled(clicked);
}

void SendLaterDialog::load(SendLaterInfo *info)
{
    mSendLaterWidget->mDateTime->setDateTime(info->dateTime());
    const bool recurrence = info->isRecurrence();
    mSendLaterWidget->mRecurrence->setChecked(recurrence);
    slotRecurrenceClicked(recurrence);
    mSendLaterWidget->mRecurrenceValue->setValue(info->recurrenceEachValue());
    mSendLaterWidget->mRecurrenceComboBox->setCurrentIndex(static_cast<int>(info->recurrenceUnit()));
}

SendLaterInfo *SendLaterDialog::info()
{
    if (!mInfo) {
        mInfo = new SendLaterInfo();
    }
    mInfo->setRecurrence(mSendLaterWidget->mRecurrence->isChecked());
    mInfo->setRecurrenceEachValue(mSendLaterWidget->mRecurrenceValue->value());
    mInfo->setRecurrenceUnit(static_cast<SendLaterInfo::RecurrenceUnit>(mSendLaterWidget->mRecurrenceComboBox->currentIndex()));
    if (mSendDateTime.isValid()) {
        mInfo->setDateTime(mSendDateTime);
    } else {
        mInfo->setDateTime(mSendLaterWidget->mDateTime->dateTime());
    }
    return mInfo;
}

SendLaterDialog::SendLaterAction SendLaterDialog::action() const
{
    return mAction;
}

void SendLaterDialog::slotOkClicked()
{
    if (!mDelay || mDelay->isChecked()) {
        mSendDateTime = mSendLaterWidget->mDateTime->dateTime();
        mAction = SendDeliveryAtTime;
    } else {
        mAction = PutInOutbox;
    }
    accept();
}

void SendLaterDialog::slotDelay(bool delayEnabled)
{
    mSendLaterWidget->mLabel->setEnabled(delayEnabled);
    mSendLaterWidget->mDateTime->setEnabled(delayEnabled);
    mSendLaterWidget->mRecurrence->setEnabled(delayEnabled);
    mSendLaterWidget->mRecurrenceValue->setEnabled(delayEnabled && mSendLaterWidget->mRecurrence->isChecked());
    mSendLaterWidget->mRecurrenceComboBox->setEnabled(delayEnabled && mSendLaterWidget->mRecurrence->isChecked());
}

#include "moc_sendlaterdialog.cpp"

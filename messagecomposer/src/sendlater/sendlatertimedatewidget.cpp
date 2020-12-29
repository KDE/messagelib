/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sendlatertimedatewidget_p.h"

#include <KTimeComboBox>
#include <KDateComboBox>
#include <KLocalizedString>
#include <QLineEdit>

#include <QHBoxLayout>

using namespace MessageComposer;

SendLaterTimeDateWidget::SendLaterTimeDateWidget(QWidget *parent)
    : QWidget(parent)
{
    auto lay = new QHBoxLayout(this);
    lay->setContentsMargins({});

    QDateTime t = QDateTime::currentDateTime();
    t = t.addSecs(60 * 60);
    mTimeComboBox.reset(new KTimeComboBox);
    connect(mTimeComboBox.get(), &KTimeComboBox::timeChanged, this, &SendLaterTimeDateWidget::slotDateTimeChanged);
    mTimeComboBox->setObjectName(QStringLiteral("time_sendlater"));

    mDateComboBox.reset(new KDateComboBox);
    mDateComboBox->setOptions(KDateComboBox::EditDate | KDateComboBox::SelectDate | KDateComboBox::DatePicker | KDateComboBox::DateKeywords | KDateComboBox::WarnOnInvalid);
    mDateComboBox->setObjectName(QStringLiteral("date_sendlater"));
    connect(mDateComboBox->lineEdit(), &QLineEdit::textChanged, this, &SendLaterTimeDateWidget::dateChanged);
    mDateComboBox->setMinimumDate(t.date(), i18n("You cannot select a date prior to the current date."));
    connect(mDateComboBox.get(), &KDateComboBox::dateChanged, this, &SendLaterTimeDateWidget::slotDateTimeChanged);

    lay->addWidget(mDateComboBox.get());
    lay->addWidget(mTimeComboBox.get());
}

SendLaterTimeDateWidget::~SendLaterTimeDateWidget() = default;

void SendLaterTimeDateWidget::slotDateTimeChanged()
{
    QDateTime dt;
    dt.setDate(mDateComboBox->date());
    dt.setTime(mTimeComboBox->time());
    Q_EMIT dateTimeChanged(dt);
}

QDateTime SendLaterTimeDateWidget::dateTime() const
{
    QDateTime dt;
    dt.setDate(mDateComboBox->date());
    dt.setTime(mTimeComboBox->time());
    return dt;
}

void SendLaterTimeDateWidget::setDateTime(const QDateTime &datetime)
{
    mTimeComboBox->setTime(datetime.time());
    mDateComboBox->setDate(datetime.date());
}

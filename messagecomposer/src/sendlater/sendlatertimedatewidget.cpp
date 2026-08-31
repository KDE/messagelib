/*
   SPDX-FileCopyrightText: 2013-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sendlatertimedatewidget_p.h"

#include <KDateComboBox>
#include <KLocalizedString>
#include <KTimeComboBox>
#include <QLineEdit>

#include <QHBoxLayout>

using namespace Qt::Literals::StringLiterals;
using namespace MessageComposer;

SendLaterTimeDateWidget::SendLaterTimeDateWidget(QWidget *parent)
    : QWidget(parent)
    , mTimeComboBox(new KTimeComboBox(this))
    , mDateComboBox(new KDateComboBox(this))
{
    auto lay = new QHBoxLayout(this);
    lay->setContentsMargins({});

    QDateTime t = QDateTime::currentDateTime();
    constexpr int minutes = 60 * 60;
    t = t.addSecs(minutes);
    connect(mTimeComboBox, &KTimeComboBox::timeChanged, this, &SendLaterTimeDateWidget::slotDateTimeChanged);
    mTimeComboBox->setObjectName("time_sendlater"_L1);

    mDateComboBox->setOptions(KDateComboBox::EditDate | KDateComboBox::SelectDate | KDateComboBox::DatePicker | KDateComboBox::DateKeywords
                              | KDateComboBox::WarnOnInvalid);
    mDateComboBox->setObjectName("date_sendlater"_L1);
    connect(mDateComboBox->lineEdit(), &QLineEdit::textChanged, this, &SendLaterTimeDateWidget::dateChanged);
    mDateComboBox->setMinimumDate(t.date(), i18n("You cannot select a date prior to the current date."));
    connect(mDateComboBox, &KDateComboBox::dateChanged, this, &SendLaterTimeDateWidget::slotDateTimeChanged);

    lay->addWidget(mDateComboBox);
    lay->addWidget(mTimeComboBox);
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

#include "moc_sendlatertimedatewidget_p.cpp"

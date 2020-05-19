/*
   Copyright (C) 2013-2020 Laurent Montel <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

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

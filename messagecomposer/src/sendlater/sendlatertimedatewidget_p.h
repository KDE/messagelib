/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QDateTime>
#include <QWidget>

class KTimeComboBox;
class KDateComboBox;

namespace MessageComposer
{
/** Send later date/time widget. */
class SendLaterTimeDateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SendLaterTimeDateWidget(QWidget *parent = nullptr);
    ~SendLaterTimeDateWidget() override;

    void setDateTime(const QDateTime &);
    [[nodiscard]] QDateTime dateTime() const;

Q_SIGNALS:
    void dateTimeChanged(const QDateTime &);
    void dateChanged(const QString &);

private:
    void slotDateTimeChanged();
    KTimeComboBox *const mTimeComboBox;
    KDateComboBox *const mDateComboBox;
};
}

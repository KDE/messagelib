/*
   SPDX-FileCopyrightText: 2013-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_SENDLATERTIMEDATEWIDGET_H
#define MESSAGECOMPOSER_SENDLATERTIMEDATEWIDGET_H

#include <QWidget>
#include <QDateTime>

#include <memory>

class KTimeComboBox;
class KDateComboBox;

namespace MessageComposer {
/** Send later date/time widget. */
class SendLaterTimeDateWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SendLaterTimeDateWidget(QWidget *parent = nullptr);
    ~SendLaterTimeDateWidget() override;

    void setDateTime(const QDateTime &);
    Q_REQUIRED_RESULT QDateTime dateTime() const;

Q_SIGNALS:
    void dateTimeChanged(const QDateTime &);
    void dateChanged(const QString &);

private:
    void slotDateTimeChanged();
    std::unique_ptr<KTimeComboBox> mTimeComboBox;
    std::unique_ptr<KDateComboBox> mDateComboBox;
};
}

#endif // MESSAGECOMPOSER_SENDLATERTIMEDATEWIDGET_H

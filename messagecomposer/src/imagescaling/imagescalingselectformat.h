/*
   SPDX-FileCopyrightText: 2013-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QDialog>

class QLineEdit;
class QPushButton;
class QListWidget;

namespace MessageComposer
{
class ImageScalingSelectFormatDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ImageScalingSelectFormatDialog(QWidget *parent);
    ~ImageScalingSelectFormatDialog() override;

    void setFormat(const QString &format);
    QString format() const;

private:
    enum { ImageRole = Qt::UserRole + 1 };
    void initialize();
    void addImageFormat(const QString &format, const QString &mimetype);
    QListWidget *mListWidget = nullptr;
};

class ImageScalingSelectFormat : public QWidget
{
    Q_OBJECT
public:
    explicit ImageScalingSelectFormat(QWidget *parent);
    ~ImageScalingSelectFormat();

    void setFormat(const QString &format);
    Q_REQUIRED_RESULT QString format() const;

Q_SIGNALS:
    void textChanged(const QString &);

private Q_SLOTS:
    void slotSelectFormat();

private:
    QLineEdit *mFormat = nullptr;
    QPushButton *mSelectFormat = nullptr;
};
}


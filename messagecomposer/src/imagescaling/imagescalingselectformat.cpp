/*
   SPDX-FileCopyrightText: 2013-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "imagescalingselectformat.h"

#include <KLocalizedString>
#include <QLineEdit>
#include <QPushButton>

#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QListWidget>
#include <QPointer>
#include <QVBoxLayout>

using namespace MessageComposer;
using namespace Qt::Literals::StringLiterals;

ImageScalingSelectFormatDialog::ImageScalingSelectFormatDialog(QWidget *parent)
    : QDialog(parent)
    , mListWidget(new QListWidget(this))
{
    setWindowTitle(i18nc("@title:window", "Select Image Format"));
    auto mainLayout = new QVBoxLayout(this);
    auto buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(mListWidget);
    mainLayout->addWidget(buttonBox);

    initialize();
}

ImageScalingSelectFormatDialog::~ImageScalingSelectFormatDialog() = default;

void ImageScalingSelectFormatDialog::addImageFormat(const QString &format, const QString &mimetype)
{
    auto item = new QListWidgetItem(format);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setData(ImageScalingSelectFormatDialog::ImageRole, mimetype);
    item->setCheckState(Qt::Unchecked);
    mListWidget->addItem(item);
}

void ImageScalingSelectFormatDialog::initialize()
{
    addImageFormat(u"PNG"_s, u"image/png"_s);
    addImageFormat(u"JPEG"_s, u"image/jpeg"_s);
    addImageFormat(u"GIF"_s, u"image/gif"_s);
    addImageFormat(u"BMP"_s, u"image/bmp"_s);
}

QString ImageScalingSelectFormatDialog::format() const
{
    const int numberOfElement(mListWidget->count());
    QString formatStr;
    for (int i = 0; i < numberOfElement; ++i) {
        if (mListWidget->item(i)->checkState() == Qt::Checked) {
            if (!formatStr.isEmpty()) {
                formatStr += u';';
            }
            formatStr += mListWidget->item(i)->data(ImageScalingSelectFormatDialog::ImageRole).toString();
        }
    }
    return formatStr;
}

void ImageScalingSelectFormatDialog::setFormat(const QString &format)
{
    const QList<QStringView> listFormat = QStringView(format).split(u';');
    const int numberOfElement(mListWidget->count());
    for (int i = 0; i < numberOfElement; ++i) {
        QListWidgetItem *item = mListWidget->item(i);
        if (listFormat.contains(item->data(ImageScalingSelectFormatDialog::ImageRole).toString())) {
            item->setCheckState(Qt::Checked);
        }
    }
}

ImageScalingSelectFormat::ImageScalingSelectFormat(QWidget *parent)
    : QWidget(parent)
    , mFormat(new QLineEdit(this))
    , mSelectFormat(new QPushButton(i18nc("@action:button", "Select Format..."), this))
{
    auto lay = new QHBoxLayout(this);
    lay->setContentsMargins({});
    connect(mFormat, &QLineEdit::textChanged, this, &ImageScalingSelectFormat::textChanged);
    mFormat->setReadOnly(true);
    lay->addWidget(mFormat);
    connect(mSelectFormat, &QPushButton::clicked, this, &ImageScalingSelectFormat::slotSelectFormat);
    lay->addWidget(mSelectFormat);
}

ImageScalingSelectFormat::~ImageScalingSelectFormat() = default;

void ImageScalingSelectFormat::slotSelectFormat()
{
    QPointer<ImageScalingSelectFormatDialog> dialog = new ImageScalingSelectFormatDialog(this);
    dialog->setFormat(mFormat->text());
    if (dialog->exec()) {
        mFormat->setText(dialog->format());
    }
    delete dialog;
}

void ImageScalingSelectFormat::setFormat(const QString &format)
{
    mFormat->setText(format);
}

QString ImageScalingSelectFormat::format() const
{
    return mFormat->text();
}

#include "moc_imagescalingselectformat.cpp"

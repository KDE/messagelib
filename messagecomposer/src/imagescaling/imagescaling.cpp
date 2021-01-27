/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "imagescaling.h"
#include "settings/messagecomposersettings.h"
#include <QBuffer>
#include <QImage>
using namespace MessageComposer;

class MessageComposer::ImageScalingPrivate
{
public:
    ImageScalingPrivate()
    {
    }

    QImage mImage;
    QBuffer mBuffer;
    QString mName;
    QByteArray mMimeType;
};

ImageScaling::ImageScaling()
    : d(new MessageComposer::ImageScalingPrivate)
{
}

ImageScaling::~ImageScaling()
{
    delete d;
}

bool ImageScaling::loadImageFromData(const QByteArray &data)
{
    if (!d->mImage.loadFromData(data)) {
        return false;
    }
    return true;
}

bool ImageScaling::resizeImage()
{
    if (d->mImage.isNull()) {
        return false;
    }
    const int width = d->mImage.width();
    const int height = d->mImage.height();
    int newWidth = -1;
    int newHeight = -1;
    if (MessageComposer::MessageComposerSettings::self()->reduceImageToMaximum()) {
        int maximumWidth = MessageComposer::MessageComposerSettings::self()->maximumWidth();
        if (maximumWidth == -1) {
            maximumWidth = MessageComposer::MessageComposerSettings::self()->customMaximumWidth();
        }
        int maximumHeight = MessageComposer::MessageComposerSettings::self()->maximumHeight();
        if (maximumHeight == -1) {
            maximumHeight = MessageComposer::MessageComposerSettings::self()->customMaximumHeight();
        }
        if (width > maximumWidth) {
            newWidth = maximumWidth;
        } else {
            newWidth = width;
        }
        if (height > maximumHeight) {
            newHeight = maximumHeight;
        } else {
            newHeight = height;
        }
    } else {
        newHeight = height;
        newWidth = width;
    }
    if (MessageComposer::MessageComposerSettings::self()->enlargeImageToMinimum()) {
        int minimumWidth = MessageComposer::MessageComposerSettings::self()->minimumWidth();
        if (minimumWidth == -1) {
            minimumWidth = MessageComposer::MessageComposerSettings::self()->customMinimumWidth();
        }

        int minimumHeight = MessageComposer::MessageComposerSettings::self()->minimumHeight();
        if (minimumHeight == -1) {
            minimumHeight = MessageComposer::MessageComposerSettings::self()->customMinimumHeight();
        }
        if (newWidth < minimumWidth) {
            newWidth = minimumWidth;
        }
        if (newHeight < minimumHeight) {
            newHeight = minimumHeight;
        }
    }
    if ((newHeight != height) || (newWidth != width)) {
        d->mBuffer.open(QIODevice::WriteOnly);
        d->mImage = d->mImage.scaled(newWidth,
                                     newHeight,
                                     MessageComposer::MessageComposerSettings::self()->keepImageRatio() ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio);

        QByteArray format;
        if (d->mMimeType == "image/jpeg") {
            format = QByteArrayLiteral("JPG");
        } else if (d->mMimeType == "image/png") {
            format = QByteArrayLiteral("PNG");
        } else {
            format = MessageComposer::MessageComposerSettings::self()->writeFormat().toLocal8Bit();
            if (format.isEmpty()) {
                format = QByteArrayLiteral("PNG");
            }
        }
        const bool result = d->mImage.save(&d->mBuffer, format.constData());
        d->mBuffer.close();
        return result;
    } else {
        return false;
    }
    return true;
}

QByteArray ImageScaling::mimetype() const
{
    if (d->mMimeType.isEmpty()) {
        return QByteArray();
    }
    if ((d->mMimeType == "image/jpeg") || (d->mMimeType == "image/png")) {
        return d->mMimeType;
    } else {
        // Add more mimetype if a day we add more saving format.
        const QString type = MessageComposer::MessageComposerSettings::self()->writeFormat();
        if (type == QLatin1String("JPG")) {
            return "image/jpeg";
        } else {
            return "image/png";
        }
    }
}

void ImageScaling::setMimetype(const QByteArray &mimetype)
{
    d->mMimeType = mimetype;
}

void ImageScaling::setName(const QString &name)
{
    d->mName = name;
}

QByteArray ImageScaling::imageArray() const
{
    return d->mBuffer.data();
}

QString ImageScaling::generateNewName()
{
    if (d->mName.isEmpty()) {
        return QString();
    }

    // Don't rename it.
    if ((d->mMimeType == "image/jpeg") || (d->mMimeType == "image/png")) {
        return d->mName;
    }
    QString type = MessageComposer::MessageComposerSettings::self()->writeFormat();
    if (type.isEmpty()) {
        type = QStringLiteral("PNG");
    }
    if (d->mName.endsWith(QLatin1String(".png"))) {
        if (type != QLatin1String("PNG")) {
            d->mName.replace(QLatin1String(".png"), QLatin1String(".jpg"));
        }
    } else if (d->mName.endsWith(QLatin1String(".jpg"))) {
        if (type != QLatin1String("JPG")) {
            d->mName.replace(QLatin1String(".jpg"), QLatin1String(".png"));
        }
    } else {
        if (type == QLatin1String("PNG")) {
            d->mName += QLatin1String(".png");
        } else {
            d->mName += QLatin1String(".jpg");
        }
    }
    return d->mName;
}

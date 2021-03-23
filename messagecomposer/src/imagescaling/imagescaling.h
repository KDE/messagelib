/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QByteArray>

namespace MessageComposer
{
class ImageScalingPrivate;
/**
 * @brief The ImageScaling class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT ImageScaling
{
public:
    ImageScaling();
    ~ImageScaling();

    /**
     * @brief loadImageFromData
     * @param data
     * @return true if we can load image.
     */
    Q_REQUIRED_RESULT bool loadImageFromData(const QByteArray &data);

    /**
     * @brief resizeImage
     * @return true if we are able to resize image
     */
    Q_REQUIRED_RESULT bool resizeImage();

    /**
     * @brief imageArray
     * @return data from image after saving
     */
    Q_REQUIRED_RESULT QByteArray imageArray() const;

    /**
     * @brief mimetype
     * @return new image mimetype after saving.
     */
    Q_REQUIRED_RESULT QByteArray mimetype() const;
    void setMimetype(const QByteArray &mimetype);

    void setName(const QString &name);

    Q_REQUIRED_RESULT QString generateNewName();

private:
    ImageScalingPrivate *const d;
};
}


/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"
#include <QByteArray>
#include <memory>

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
    [[nodiscard]] bool loadImageFromData(const QByteArray &data);

    /**
     * @brief resizeImage
     * @return true if we are able to resize image
     */
    [[nodiscard]] bool resizeImage();

    /**
     * @brief imageArray
     * @return data from image after saving
     */
    [[nodiscard]] QByteArray imageArray() const;

    /**
     * @brief mimetype
     * @return new image mimetype after saving.
     */
    [[nodiscard]] QByteArray mimetype() const;
    void setMimetype(const QByteArray &mimetype);

    void setName(const QString &name);

    [[nodiscard]] QString generateNewName();

private:
    std::unique_ptr<ImageScalingPrivate> const d;
};
}

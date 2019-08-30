/*
   Copyright (C) 2012-2019 Montel Laurent <montel@kde.org>

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

#ifndef IMAGESCALING_H
#define IMAGESCALING_H

#include "messagecomposer_export.h"
#include <QByteArray>

namespace MessageComposer {
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

#endif // IMAGESCALING_H

// SPDX-CopyrightText: 2026 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: LGPL-2.0-or-later

#pragma once

#include <QByteArray>
#include <QList>
#include <QMetaType>
#include <QString>

namespace MessageComposer
{
/*!
 * \struct MessageComposer::EmbeddedImage
 * \inmodule MessageComposer
 * \inheaderfile MessageComposer/EmbeddedImage
 *
 * Image data for a related MIME part referenced by an HTML message.
 */
struct EmbeddedImage {
    /*!
     * \variable MessageComposer::EmbeddedImage::image
     * PNG data, already encoded as base64.
     */
    QByteArray image;

    /*!
     * \variable MessageComposer::EmbeddedImage::contentID
     * Content-ID of the image MIME part, without angle brackets.
     */
    QString contentID;

    /*!
     * \variable MessageComposer::EmbeddedImage::imageName
     * Image name referenced in the HTML before conversion to a cid URL.
     */
    QString imageName;
};

using EmbeddedImageList = QList<EmbeddedImage>;
}

Q_DECLARE_METATYPE(MessageComposer::EmbeddedImage)

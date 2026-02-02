/*
  SPDX-FileCopyrightText: 2017 Sandro Kanuß <sknauss@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser/objecttreesource.h"
#include <memory>

namespace MimeTreeParser
{
class SimpleObjectTreeSourcePrivate;

/**
 * \class MimeTreeParser::SimpleObjectTreeSource
 * \inmodule MimeTreeParser
 * \inheaderfile MimeTreeParser/SimpleObjectTreeSource
 *
 * A very simple ObjectTreeSource.
 * mostly used in tests and outside the read rendering
 */
class MIMETREEPARSER_EXPORT SimpleObjectTreeSource : public Interface::ObjectTreeSource
{
public:
    /*!
     * \brief Constructor.
     */
    SimpleObjectTreeSource();
    /*!
     * \brief Destructor.
     */
    ~SimpleObjectTreeSource() override;

    /*!
     * \brief Check if auto import keys.
     * \return True if auto import keys is enabled.
     */
    [[nodiscard]] bool autoImportKeys() const override;

    /*!
     * \brief Get body part formatter factory.
     * \return Pointer to the BodyPartFormatterFactory.
     */
    const BodyPartFormatterFactory *bodyPartFormatterFactory() override;
    /*!
     * \brief Check if should decrypt message.
     * \return True if message should be decrypted.
     */
    [[nodiscard]] bool decryptMessage() const override;
    /*!
     * \brief Set whether to decrypt message.
     * \param decryptMessage Whether to decrypt the message.
     */
    void setDecryptMessage(bool decryptMessage);

    /*!
     * \brief Set HTML mode.
     * \param mode The HTML mode to set.
     * \param availableModes The list of available HTML modes.
     */
    void setHtmlMode(MimeTreeParser::Util::HtmlMode mode, const QList<MimeTreeParser::Util::HtmlMode> &availableModes) override;

    /*!
     * \brief Set preferred mode.
     * \param mode The preferred HTML mode to set.
     */
    void setPreferredMode(MimeTreeParser::Util::HtmlMode mode);
    /*!
     */
    [[nodiscard]] MimeTreeParser::Util::HtmlMode preferredMode() const override;

    /*!
     */
    void setOverrideCodecName(const QByteArray &codec);
    /*!
     */
    [[nodiscard]] QByteArray overrideCodecName() const override;

private:
    std::unique_ptr<SimpleObjectTreeSourcePrivate> d;
};
}

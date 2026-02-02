/*
    partnodebodypart.h

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2004 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2004 Ingo Kloecker <kloecker@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "mimetreeparser/bodypart.h"
#include "mimetreeparser_export.h"

namespace KMime
{
class Content;
}

namespace MimeTreeParser
{
class NodeHelper;
}

namespace MimeTreeParser
{
/**
 * \class MimeTreeParser::PartNodeBodyPart
 * \inmodule MimeTreeParser
 * \inheaderfile MimeTreeParser/PartNodeBodyPart
 *
 * @short an implementation of the BodyPart interface using KMime::Content's
 */
class MIMETREEPARSER_EXPORT PartNodeBodyPart : public Interface::BodyPart
{
public:
    /*!
     * \brief Constructor.
     * \param otp The ObjectTreeParser.
     * \param result The ProcessResult.
     * \param topLevelContent The top-level KMime::Content.
     * \param content The KMime::Content.
     * \param nodeHelper The NodeHelper.
     */
    explicit PartNodeBodyPart(ObjectTreeParser *otp, ProcessResult *result, KMime::Content *topLevelContent, KMime::Content *content, NodeHelper *nodeHelper);

    /*!
     * \brief Get memento.
     * \return The BodyPartMemento.
     */
    Interface::BodyPartMemento *memento() const override;
    /*!
     * \brief Get content.
     * \return The KMime::Content.
     */
    KMime::Content *content() const override;
    /*!
     * \brief Get top-level content.
     * \return The top-level KMime::Content.
     */
    KMime::Content *topLevelContent() const override;
    /*!
     * \brief Get process result.
     * \return The ProcessResult.
     */
    ProcessResult *processResult() const override;

    /*!
     */
    NodeHelper *nodeHelper() const override;
    /*!
     */
    ObjectTreeParser *objectTreeParser() const override;
    /*!
     */
    Interface::ObjectTreeSource *source() const override;

private:
    KMime::Content *const mTopLevelContent;
    KMime::Content *const mContent;
    NodeHelper *const mNodeHelper;
    ObjectTreeParser *const mObjectTreeParser;
    ProcessResult *const mProcessResult;
};
}

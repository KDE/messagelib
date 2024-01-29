/*  -*- c++ -*-
    attachmentstrategy.cpp

    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>
    SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
    SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "attachmentstrategy.h"

#include <MimeTreeParser/NodeHelper>
#include <MimeTreeParser/Util>

#include <KMime/Content>

#include <QIcon>

#include "messageviewer_debug.h"

using namespace MessageViewer;

static AttachmentStrategy::Display smartDisplay(KMime::Content *node)
{
    const auto cd = node->contentDisposition(false);
    if (cd) {
        if (cd->disposition() == KMime::Headers::CDinline) {
            // explicit "inline" disposition:
            return AttachmentStrategy::Inline;
        }
        if (cd->disposition() == KMime::Headers::CDattachment) {
            // explicit "attachment" disposition:
            return AttachmentStrategy::AsIcon;
        }
    }

    const auto ct = node->contentType(false);
    if (ct && ct->isText() && ct->name().trimmed().isEmpty() && (!cd || cd->filename().trimmed().isEmpty())) {
        // text/* w/o filename parameter:
        return AttachmentStrategy::Inline;
    }
    return AttachmentStrategy::AsIcon;
}

//
// IconicAttachmentStrategy:
//   show everything but the first text/plain body as icons
//

class IconicAttachmentStrategy : public AttachmentStrategy
{
    friend class AttachmentStrategy;

protected:
    IconicAttachmentStrategy()
        : AttachmentStrategy()
    {
    }

    ~IconicAttachmentStrategy() override = default;

public:
    [[nodiscard]] const char *name() const override
    {
        return "iconic";
    }

    [[nodiscard]] bool inlineNestedMessages() const override
    {
        return false;
    }

    Display defaultDisplay(KMime::Content *node) const override
    {
        if (node->contentType()->isText()
            && (!node->parent() || (node->contentDisposition()->filename().trimmed().isEmpty() && node->contentType(false)->name().trimmed().isEmpty()))) {
            // text/* w/o filename parameter:
            return Inline;
        }
        return AsIcon;
    }
};

//
// SmartAttachmentStrategy:
//   in addition to Iconic, show all body parts
//   with content-disposition == "inline" and
//   all text parts without a filename or name parameter inline
//

class SmartAttachmentStrategy : public AttachmentStrategy
{
    friend class AttachmentStrategy;

protected:
    SmartAttachmentStrategy()
        : AttachmentStrategy()
    {
    }

    ~SmartAttachmentStrategy() override = default;

public:
    [[nodiscard]] const char *name() const override
    {
        return "smart";
    }

    [[nodiscard]] bool inlineNestedMessages() const override
    {
        return true;
    }

    Display defaultDisplay(KMime::Content *node) const override
    {
        return smartDisplay(node);
    }
};

//
// InlinedAttachmentStrategy:
//   show everything possible inline
//

class InlinedAttachmentStrategy : public AttachmentStrategy
{
    friend class AttachmentStrategy;

protected:
    InlinedAttachmentStrategy()
        : AttachmentStrategy()
    {
    }

    ~InlinedAttachmentStrategy() override = default;

public:
    [[nodiscard]] const char *name() const override
    {
        return "inlined";
    }

    [[nodiscard]] bool inlineNestedMessages() const override
    {
        return true;
    }

    Display defaultDisplay(KMime::Content *) const override
    {
        return Inline;
    }
};

//
// HiddenAttachmentStrategy
//   show nothing except the first text/plain body part _at all_
//

class HiddenAttachmentStrategy : public AttachmentStrategy
{
    friend class AttachmentStrategy;

protected:
    HiddenAttachmentStrategy()
        : AttachmentStrategy()
    {
    }

    ~HiddenAttachmentStrategy() override = default;

public:
    [[nodiscard]] const char *name() const override
    {
        return "hidden";
    }

    [[nodiscard]] bool inlineNestedMessages() const override
    {
        return false;
    }

    Display defaultDisplay(KMime::Content *node) const override
    {
        if (node->contentType()->isText() && node->contentDisposition()->filename().trimmed().isEmpty()
            && node->contentType(false)->name().trimmed().isEmpty()) {
            // text/* w/o filename parameter:
            return Inline;
        }
        if (!node->parent()) {
            return Inline;
        }

        if (node->parent() && node->parent()->contentType()->isMultipart() && node->parent()->contentType(false)->subType() == "related") {
            return Inline;
        }

        return None;
    }
};

class HeaderOnlyAttachmentStrategy : public AttachmentStrategy
{
    friend class AttachmentStrategy;

protected:
    HeaderOnlyAttachmentStrategy()
        : AttachmentStrategy()
    {
    }

    ~HeaderOnlyAttachmentStrategy() override = default;

public:
    [[nodiscard]] const char *name() const override
    {
        return "headerOnly";
    }

    [[nodiscard]] bool inlineNestedMessages() const override
    {
        return true;
    }

    Display defaultDisplay(KMime::Content *node) const override
    {
        if (MimeTreeParser::NodeHelper::isInEncapsulatedMessage(node)) {
            return smartDisplay(node);
        }

        if (!MimeTreeParser::Util::labelForContent(node).isEmpty() && QIcon::hasThemeIcon(MimeTreeParser::Util::iconNameForContent(node))
            && !MimeTreeParser::Util::isTypeBlacklisted(node)) {
            return None;
        }
        return smartDisplay(node);
    }

    [[nodiscard]] bool requiresAttachmentListInHeader() const override
    {
        return true;
    }
};

//
// AttachmentStrategy abstract base:
//

AttachmentStrategy::AttachmentStrategy() = default;

AttachmentStrategy::~AttachmentStrategy() = default;

const AttachmentStrategy *AttachmentStrategy::create(Type type)
{
    switch (type) {
    case Iconic:
        return iconic();
    case Smart:
        return smart();
    case Inlined:
        return inlined();
    case Hidden:
        return hidden();
    case HeaderOnly:
        return headerOnly();
    }
    qCCritical(MESSAGEVIEWER_LOG) << "Unknown attachment strategy ( type ==" << static_cast<int>(type) << ") requested!";
    return nullptr; // make compiler happy
}

const AttachmentStrategy *AttachmentStrategy::create(const QString &type)
{
    const QString lowerType = type.toLower();
    if (lowerType == QLatin1StringView("iconic")) {
        return iconic();
    }
    // if ( lowerType == "smart" )    return smart(); // not needed, see below
    if (lowerType == QLatin1StringView("inlined")) {
        return inlined();
    }
    if (lowerType == QLatin1StringView("hidden")) {
        return hidden();
    }
    if (lowerType == QLatin1StringView("headeronly")) {
        return headerOnly();
    }
    // don't kFatal here, b/c the strings are user-provided
    // (KConfig), so fail gracefully to the default:
    return smart();
}

static const AttachmentStrategy *iconicStrategy = nullptr;
static const AttachmentStrategy *smartStrategy = nullptr;
static const AttachmentStrategy *inlinedStrategy = nullptr;
static const AttachmentStrategy *hiddenStrategy = nullptr;
static const AttachmentStrategy *headerOnlyStrategy = nullptr;

const AttachmentStrategy *AttachmentStrategy::iconic()
{
    if (!iconicStrategy) {
        iconicStrategy = new IconicAttachmentStrategy();
    }
    return iconicStrategy;
}

const AttachmentStrategy *AttachmentStrategy::smart()
{
    if (!smartStrategy) {
        smartStrategy = new SmartAttachmentStrategy();
    }
    return smartStrategy;
}

const AttachmentStrategy *AttachmentStrategy::inlined()
{
    if (!inlinedStrategy) {
        inlinedStrategy = new InlinedAttachmentStrategy();
    }
    return inlinedStrategy;
}

const AttachmentStrategy *AttachmentStrategy::hidden()
{
    if (!hiddenStrategy) {
        hiddenStrategy = new HiddenAttachmentStrategy();
    }
    return hiddenStrategy;
}

const AttachmentStrategy *AttachmentStrategy::headerOnly()
{
    if (!headerOnlyStrategy) {
        headerOnlyStrategy = new HeaderOnlyAttachmentStrategy();
    }
    return headerOnlyStrategy;
}

bool AttachmentStrategy::requiresAttachmentListInHeader() const
{
    return false;
}

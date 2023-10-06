/*
    This file is part of KMail, the KDE mail client.
    SPDX-FileCopyrightText: 2017 Sandro Knauß <sknauss@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <MimeTreeParser/MessagePart>

#include <QList>

#include <memory>

namespace MessageViewer
{
class MessagePartRendererBase;
class MessagePartRendererFactoryPrivate;
/**
 * @brief The MessagePartRendererFactory class
 */
class MESSAGEVIEWER_EXPORT MessagePartRendererFactory
{
public:
    MessagePartRendererFactory();
    ~MessagePartRendererFactory();

    /** Customize where to look for render plugins.
     *  This is the sub directory appended to the Qt plugin search paths.
     *  An empty string disables plugin loading.
     *  This only has any effect before the first call to renderersForPart().
     */
    void setPluginPath(const QString &subdir);

    static MessagePartRendererFactory *instance();

    [[nodiscard]] QList<MessagePartRendererBase *> renderersForPart(const QMetaObject *mo, const MimeTreeParser::MessagePart::Ptr &mp) const;

private:
    Q_DISABLE_COPY(MessagePartRendererFactory)
    std::unique_ptr<MessagePartRendererFactoryPrivate> d;
};
}

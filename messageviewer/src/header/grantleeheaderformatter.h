/*
   SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"
#include <GrantleeTheme/GrantleeTheme>
#include <KTextTemplate/TemplateLoader>
#include <QString>
namespace KMime
{
class Message;
}

namespace MessageViewer
{
class HeaderStyle;
/**
 * @brief The GrantleeHeaderFormatter class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGEVIEWER_EXPORT GrantleeHeaderFormatter
{
public:
    struct GrantleeHeaderFormatterSettings {
        GrantleeHeaderFormatterSettings() = default;

        GrantleeTheme::Theme theme;
        bool isPrinting = false;
        mutable const MessageViewer::HeaderStyle *style = nullptr;
        KMime::Message *message = nullptr;
        bool showEmoticons = true;
    };

    explicit GrantleeHeaderFormatter();
    ~GrantleeHeaderFormatter();

    [[nodiscard]] QString toHtml(const GrantleeHeaderFormatterSettings &settings) const;

    [[nodiscard]] QString toHtml(const QStringList &displayExtraHeaders,
                                 const QString &absolutPath,
                                 const QString &filename,
                                 const MessageViewer::HeaderStyle *style,
                                 KMime::Message *message,
                                 bool isPrinting) const;

private:
    [[nodiscard]] MESSAGEVIEWER_NO_EXPORT QString format(const QString &absolutePath,
                                                         const KTextTemplate::Template &headerTemplate,
                                                         const QStringList &displayExtraHeaders,
                                                         bool isPrinting,
                                                         const MessageViewer::HeaderStyle *style,
                                                         KMime::Message *message,
                                                         bool showEmoticons = true) const;
    class GrantleeHeaderFormatterPrivate;
    std::unique_ptr<GrantleeHeaderFormatterPrivate> const d;
};
}

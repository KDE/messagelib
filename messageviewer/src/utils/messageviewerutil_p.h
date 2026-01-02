/*
 * SPDX-FileCopyrightText: 2005 Till Adam <adam@kde.org>
 * SPDX-License-Identifier: GPL-2.0-or-later WITH LicenseRef-Qt-Commercial-exception-1.0
 */

#pragma once

#include <KMime/Content>
class QUrl;
class QWidget;
class QAction;

namespace MessageViewer
{
/**
 * The Util namespace contains a collection of helper functions use in
 * various places.
 */
namespace Util
{
[[nodiscard]] bool handleUrlWithQDesktopServices(const QUrl &url);

[[nodiscard]] bool saveContents(QWidget *parent, const QList<KMime::Content *> &contents, QList<QUrl> &urlList);
[[nodiscard]] bool saveContent(QWidget *parent, const KMime::Content *content, const QUrl &url);
void addHelpTextAction(QAction *act, const QString &text);
void readGravatarConfig();
}
}

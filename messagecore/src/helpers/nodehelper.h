/*
  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>
  SPDX-FileCopyrightText: 2010 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messagecore_export.h"

namespace KMime
{
class Content;
class Message;
}

namespace MessageCore
{
/**
 * @short Contains some static functions for navigating in KMime::Node trees.
 */
namespace NodeHelper
{
/**
 * Returns the next sibling node of the given @p node.
 * If there is no sibling node @c 0 is returned.
 */
MESSAGECORE_EXPORT KMime::Content *nextSibling(const KMime::Content *node);

/**
 * Returns the next node (child, sibling or parent) of the given @p node.
 *
 * @param node The start node for iteration.
 * @param allowChildren If @c true child nodes will be returned, otherwise only sibling or parent nodes.
 */
MESSAGECORE_EXPORT KMime::Content *next(KMime::Content *node, bool allowChildren = true);

/**
 * Returns the first child node of the given @p node.
 * If there is no child node @c 0 is returned.
 */
MESSAGECORE_EXPORT KMime::Content *firstChild(const KMime::Content *node);
}
}


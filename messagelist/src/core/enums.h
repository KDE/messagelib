/******************************************************************************
 *
 *  SPDX-FileCopyrightText: 2008 Szymon Tomasz Stefanek <pragma@kvirc.net>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 *******************************************************************************/

#pragma once

namespace MessageList
{
namespace Core
{
/**
 * Pre-selection is the action of automatically selecting a message just after the folder
 * has finished loading. We may want to select the message that was selected the last
 * time this folder has been open, or we may want to select the first unread message.
 * We also may want to do no pre-selection at all (for example, when the user
 * starts navigating the view before the pre-selection could actually be made
 * and pre-selecting would confuse him). This member holds the option.
 *
 * All the modes except PreSelectNone try to fallback to "PreSelectLastSelected" when the
 * specified item isn't found.
 *
 * See Model::setStorageModel() for more information.
 */
enum PreSelectionMode {
    PreSelectNone, //< no pre-selection at all
    PreSelectLastSelected, //< pre-select the last message that was selected in this folder (default)
    PreSelectFirstUnreadCentered, //< pre-select the first unread message and center it
    PreSelectNewestCentered, //< pre-select the newest message, by date
    PreSelectOldestCentered //< pre-select the oldest message, by date
};

/**
 * This enum is used in the view message selection functions (for instance View::nextMessageItem()).
 */
enum MessageTypeFilter {
    MessageTypeAny, //< Select any message
    MessageTypeUnreadOnly //< Select only unread messages
};

/**
 * This enum is used in the view message selection functions (for instance View::selectNextMessage())
 */
enum ExistingSelectionBehaviour {
    ClearExistingSelection, //< Clear the existing selection before selecting the new item
    ExpandExistingSelection, //< Preserve the existing selection (grow only)
    GrowOrShrinkExistingSelection //< Grow or shrink the existing selection depending on what item is actually being selected
};
}
}


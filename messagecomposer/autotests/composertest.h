/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>
  SPDX-FileCopyrightText: 2009 Leo Franchi <lfranchi@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>

namespace MessageComposer
{
class Composer;
}

class ComposerTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testAttachments();
    void testAutoSave();
    void testNonAsciiHeaders();
    void testBug271192();

    // TODO test the code for autodetecting the charset of a text attachment.
    // TODO figure out what CTE testing has to be done.
private:
    void fillComposerData(MessageComposer::Composer *composer);
};

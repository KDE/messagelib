/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGECOMPOSER_MESSAGEPART_H
#define MESSAGECOMPOSER_MESSAGEPART_H

#include "messagecomposer_export.h"

#include <QObject>

#include <kmime/kmime_headers.h>

namespace MessageComposer {
class MessagePartPrivate;
/**
 * @brief The MessagePart class
 */
class MESSAGECOMPOSER_EXPORT MessagePart : public QObject
{
    Q_OBJECT

public:
    explicit MessagePart(QObject *parent = nullptr);
    ~MessagePart() override;

private:
    class Private;
    Private *const d;
};
}

#endif

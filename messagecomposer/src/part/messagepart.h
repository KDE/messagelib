/*
  SPDX-FileCopyrightText: 2009 Constantin Berzan <exit3219@gmail.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messagecomposer_export.h"

#include <QObject>

#include <KMime/Headers>

namespace MessageComposer
{
/**
 * @brief The MessagePart class
 */
class MESSAGECOMPOSER_EXPORT MessagePart : public QObject
{
    Q_OBJECT

public:
    explicit MessagePart(QObject *parent = nullptr);
    ~MessagePart() override;
};
}

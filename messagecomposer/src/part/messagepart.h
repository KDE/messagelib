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
/*!
 \class MessageComposer::MessagePart
 \inmodule MessageComposer
 \inheaderfile MessageComposer/MessagePart

 The MessagePart class.
 */
class MESSAGECOMPOSER_EXPORT MessagePart : public QObject
{
    Q_OBJECT

public:
    /*! \brief Constructs a MessagePart.
        \param parent The parent object.
    */
    explicit MessagePart(QObject *parent = nullptr);
    /*! \brief Destroys the MessagePart. */
    ~MessagePart() override;
};
}

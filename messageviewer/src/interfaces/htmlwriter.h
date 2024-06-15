/*  -*- c++ -*-
    interfaces/htmlwriter.h

    This file is part of KMail's plugin interface.
    SPDX-FileCopyrightText: 2003 Marc Mutz <mutz@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "messageviewer_export.h"

#include <QByteArray>
#include <memory>
#include <qglobal.h>
class QIODevice;
class QString;
class QTextStream;

namespace MessageViewer
{
/**
 * @short An interface for HTML sinks.
 * @author Marc Mutz <mutz@kde.org>
 *
 */
class MESSAGEVIEWER_EXPORT HtmlWriter
{
public:
    HtmlWriter();
    virtual ~HtmlWriter();

    /** Signal the begin of stuff to write.
     *  Sub-classes should open device() in a writable mode here and then
     *  call the base class.
     */
    virtual void begin();

    /** Write out a chunk of text. No HTML escaping is performed.
     *  @deprecated use stream() instead
     */
    void write(const QString &html);

    /** Signal the end of stuff to write.
     *  Sub-classes should call the base class and then close device() here.
     */
    virtual void end();

    /**
     * Stop all possibly pending processing in order to be able to
     * call #begin() again.
     * Sub-classes should call the base class and then reset device() here.
     */
    virtual void reset();

    /** Returns the QIODevice backing this HtmlWriter instance.
     *  Before writing to this directly, make sure to flush stream().
     */
    virtual QIODevice *device() const = 0;

    /** Returns a QTextStream on device().
     *  Use this for writing QString data, rather than local string
     *  concatenations.
     */
    QTextStream *stream() const;

    /**
     * Embed a part with Content-ID @p contentId, using url @p url.
     */
    virtual void embedPart(const QByteArray &contentId, const QString &url) = 0;

    virtual void setExtraHead(const QString &str) = 0;

    virtual void setStyleBody(const QString &styleBody) = 0;

private:
    Q_DISABLE_COPY(HtmlWriter)
    mutable std::unique_ptr<QTextStream> m_stream;
};
}

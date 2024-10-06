// SPDX-FileCopyrightText: 2001 the KMime authors.
// SPDX-License-Identifier: LGPL-2.0-or-later

/**
  @file
  This file is part of the API for handling @ref MIME data and
  defines the DateFormatter class.

  @brief
  Defines the DateFormatter class.

  @glossary @anchor RFC2822 @anchor rfc2822 @b RFC @b 2822:
  RFC that defines the <a href="https://tools.ietf.org/html/rfc2822">
  Internet Message Format</a>.

  @glossary @anchor ISO8601 @anchor iso8601 @b ISO @b 8601:
  International Standards Organization (ISO) standard that defines the
  <a href="https://en.wikipedia.org/wiki/ISO_8601">
  international standard for date and time representations</a>.

  @glossary @anchor ctime @b ctime:
  a Unix library call which returns the local time as a human readable
  ASCII string of the form "Sun Mar 31 02:08:35 2002".
*/

#pragma once

#include "messagecore_export.h"
#include <QDateTime>
#include <QString>
#include <memory>

namespace MessageCore
{
class DateFormatterPrivate;

/**
  @brief
  A class for abstracting date formatting.

  This class deals with different kinds of date display formats.
  The formats supported include:
  - @b fancy "Today 02:08:35"
  - @b ctime as with the @ref ctime function, eg. "Sun Mar 31 02:08:35 2002"
  - @b localized according to the control center setting, eg. "2002-03-31 02:08"
  - @b custom "whatever you like"

  @note
  If a long-lived instance of this class is kept, its owner is responsible for
  calling `invalidateReferenceDate()` on date changes.
  This ensures that the `Fancy` formatter has an up-to-date notion of "Today",
  "Yesterday" etc.
*/
class MESSAGECORE_EXPORT DateFormatter
{
public:
    /**
      The different types of date formats.
    */
    enum FormatType {
        CTime, /**< ctime "Sun Mar 31 02:08:35 2002" */
        Localized, /**< localized "2002-03-31 02:08" */
        Fancy, /**< fancy "Today 02:08:35" */
        Custom /**< custom "whatever you like" */
    };

    /**
      Constructs a date formatter with a default #FormatType.

      @param ftype is the default #FormatType to use.
    */
    explicit DateFormatter(FormatType ftype = DateFormatter::Fancy);

    /**
      Destroys the date formatter.
    */
    ~DateFormatter();

    /**
      Returns the #FormatType currently set.

      @see setFormat().
    */
    [[nodiscard]] FormatType format() const;

    /**
      Sets the date format to @p ftype.

      @param ftype is the #FormatType.

      @see format().
    */
    void setFormat(FormatType ftype);

    /**
      Constructs a formatted date string from QDateTime @p dtime.

      @param dtime is the QDateTime to use for formatting.
      @param lang is the language, only used if #FormatType is #Localized.
      @param shortFormat if true, create the short version of the date string,
      only used if #FormatType is #Localized.

      @return a QString containing the formatted date.
    */
    [[nodiscard]] QString dateString(const QDateTime &dtime, const QString &lang = QString(), bool shortFormat = true) const;

    /**
      Sets the custom format for date to string conversions to @p format.
      This method accepts the same arguments as QDateTime::toString(), but
      also supports the "Z" expression which is substituted with the
      @ref RFC2822 (Section 3.3) style numeric timezone (-0500).

      @param format is a QString containing the custom format.

      @see QDateTime::toString(), customFormat().
    */
    void setCustomFormat(const QString &format);

    /**
      Returns the custom format string.

      @see setCustomFormat().
    */
    [[nodiscard]] QString customFormat() const;

    /**
      Invalidates the cached reference date used by the `Fancy` formatter to
      determine the meaning of "Today", "Yesterday" etc.
     */
    void invalidateReferenceDate() const;

    // static methods
    /**
      Convenience function dateString

      @param ftype is the #FormatType to use.
      @param t is the time to use for formatting.
      @param data is either the format when #FormatType is Custom,
      or language when #FormatType is #Localized.
      @param shortFormat if true, create the short version of the date string,
      only used if #FormatType is #Localized.

      @return a QString containing the formatted date.
    */
    [[nodiscard]] static QString formatDate(DateFormatter::FormatType ftype, const QDateTime &t, const QString &data = QString(), bool shortFormat = true);

    /**
      Convenience function, same as formatDate() but returns the current time
      formatted.

      @param ftype is the #FormatType to use.
      @param data is either the format when #FormatType is Custom,
      or language when #FormatType is #Localized.
      @param shortFormat if true, create the short version of the date string,
      only used if #FormatType is #Localized.

      @return a QString containing the formatted date.
    */
    [[nodiscard]] static QString formatCurrentDate(DateFormatter::FormatType ftype, const QString &data = QString(), bool shortFormat = true);

private:
    //@cond PRIVATE
    Q_DISABLE_COPY(DateFormatter)
    std::unique_ptr<DateFormatterPrivate> const d;
    //@endcond
};

} // namespace KMime

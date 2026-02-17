/*
   SPDX-FileCopyrightText: 2019-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once
#include "messagecomposer_export.h"
#include <QString>

namespace MessageComposer
{
/*! \namespace ConvertSnippetVariablesUtil
    \inmodule MessageComposer
    \brief Utilities for converting snippet template variables to their values.

    This namespace provides functions to convert template variables used in message snippets
    (like \\cc, \\to, \\date, etc.) into their actual values for a specific message.
    It also defines the VariableType enum for identifying different template variables.
*/
namespace ConvertSnippetVariablesUtil
{
/*! \enum VariableType
    \brief Enumeration of template variables supported in message snippets.

    These variables can be used in message templates and will be replaced with
    appropriate values when the snippet is inserted into a message.
*/
enum VariableType : uint8_t {
    CcAddr, /*!< CC field address. */
    CcLname, /*!< CC recipient last name. */
    CcFname, /*!< CC recipient first name. */
    CcName, /*!< CC recipient full name. */
    BccAddr, /*!< BCC field address. */
    BccLname, /*!< BCC recipient last name. */
    BccFname, /*!< BCC recipient first name. */
    BccName, /*!< BCC recipient full name. */

    FullSubject, /*!< Message subject line. */
    ToAddr, /*!< To field address. */
    ToFname, /*!< Recipient first name. */
    ToLname, /*!< Recipient last name. */
    ToName, /*!< Recipient full name. */
    FromAddr, /*!< Sender address. */
    FromLname, /*!< Sender last name. */
    FromFname, /*!< Sender first name. */
    FromName, /*!< Sender full name. */
    Dow, /*!< Day of week. */
    AttachmentName, /*!< Name of first attachment. */
    Date, /*!< Current date (long format). */
    ShortDate, /*!< Current date (short format). */
    CustomDate, /*!< Custom date format. */
    Time, /*!< Current time (short format). */
    TimeLong, /*!< Current time (long format). */
    AttachmentCount, /*!< Number of attachments. */
    AttachmentFilenames, /*!< List of attachment filenames. */
    AttachmentNamesAndSizes, /*!< List of attachments with sizes. */
    Year, /*!< Current year. */
    LastYear, /*!< Previous year. */
    NextYear, /*!< Next year. */
    MonthNumber, /*!< Current month number (1-12). */
    DayNumber, /*!< Current day of month. */
    DayOfMonth, /*!< Day of month. */
    WeekNumber, /*!< Week number of the year. */
    MonthNameShort, /*!< Abbreviated month name. */
    MonthNameLong, /*!< Full month name. */
    DayOfWeek, /*!< Day of week name. */
    DayOfWeekNameShort, /*!< Abbreviated day of week name. */
    DayOfWeekNameLong, /*!< Full day of week name. */
    YearLastMonth, /*!< Year of last month. */
    LastMonthNameLong, /*!< Full name of last month. */
};

/*! \brief Converts a VariableType enum value to its string representation.
    \param type The variable type to convert.
    \return The string representation of the variable type.
*/
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::VariableType type);

/*! \brief Returns a formatted string for the short date variable. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString shortDate();

/*! \brief Returns a formatted string for the long date variable. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString longDate();

/*! \brief Returns a formatted string for the short time variable. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString shortTime();

/*! \brief Returns a formatted string for the long time variable. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString longTime();

/*! \brief Returns the day of week. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString insertDayOfWeek();

/*! \brief Returns the year from last year. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString lastYear();

/*! \brief Returns the year of next year. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString nextYear();

/*! \brief Returns the day number of the month. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString dayNumber();

/*! \brief Returns the month number (1-12). */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString monthNumber();

/*! \brief Returns the current date in custom format. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString customDate();

/*! \brief Returns the day of the month. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString dayOfMonth();

/*! \brief Returns the week number of the year. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString weekNumber();

/*! \brief Returns the abbreviated month name. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString monthNameShort();

/*! \brief Returns the full month name. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString monthNameLong();

/*! \brief Returns the abbreviated day of week name. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString dayOfWeekNameShort();

/*! \brief Returns the full day of week name. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString dayOfWeekNameLong();

/*! \brief Returns the year of the last month. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString yearLastMonth();

/*! \brief Returns the current year. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString year();

/*! \brief Returns the day of week. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString dayOfWeek();

/*! \brief Returns the full name of the last month. */
[[nodiscard]] MESSAGECOMPOSER_EXPORT QString lastMonthNameLong();
}
}

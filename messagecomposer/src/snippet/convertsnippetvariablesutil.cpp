/*
   Copyright (C) 2019 Laurent Montel <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "convertsnippetvariablesutil.h"

QString MessageComposer::ConvertSnippetVariablesUtil::snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::VariableType type)
{
    switch (type) {
    case CcAddr:
        return QLatin1String("%CCADDR");
    case FullSubject:
        return QLatin1String("%FULLSUBJECT");
    case ToAddr:
        return QLatin1String("%TOADDR");
    case ToFname:
        return QLatin1String("%TOFNAME");
    case ToLname:
        return QLatin1String("%TOLNAME");
    case FromAddr:
        return QLatin1String("%FROMADDR");
    case FromLname:
        return QLatin1String("%FROMLNAME");
    case FromFname:
        return QLatin1String("%FROMFNAME");
    case CcLname:
        return QLatin1String("%CCLNAME");
    case CcFname:
        return QLatin1String("%CCFNAME");
    case Dow:
        return QLatin1String("%DOW");
    case Date:
        return QLatin1String("%DATE");
    case ShortDate:
        return QLatin1String("%SHORTDATE");
    case Time:
        return QLatin1String("%TIME");
    case TimeLong:
        return QLatin1String("%TIMELONG");
    case AttachmentCount:
        return QLatin1String("%ATTACHMENTCOUNT");
    case AttachmentName:
        return QLatin1String("%ATTACHMENTNAMES");
    case AttachmentFilenames:
        return QLatin1String("%ATTACHMENTFILENAMES");
    case AttachmentNamesAndSizes:
        return QLatin1String("%ATTACHMENTNAMESANDSIZES");
    }
    return {};
}

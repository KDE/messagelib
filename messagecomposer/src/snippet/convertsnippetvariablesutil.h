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

#ifndef CONVERTSNIPPETVARIABLESUTIL_H
#define CONVERTSNIPPETVARIABLESUTIL_H
#include "messagecomposer_export.h"
#include <QString>

namespace MessageComposer {
namespace ConvertSnippetVariablesUtil {
enum VariableType {
    CcAddr,
    CcLname,
    CcFname,
    FullSubject,
    ToAddr,
    ToFname,
    ToLname,
    FromAddr,
    FromLname,
    FromFname,
    Dow,
    AttachmentName,
    Date,
    ShortDate,
    Time,
    TimeLong,
    AttachmentCount,
    AttachmentFilenames,
    AttachmentNamesAndSizes,
};

Q_REQUIRED_RESULT MESSAGECOMPOSER_EXPORT QString snippetVariableFromEnum(MessageComposer::ConvertSnippetVariablesUtil::VariableType type);
}
}
#endif // CONVERTSNIPPETVARIABLESUTIL_H

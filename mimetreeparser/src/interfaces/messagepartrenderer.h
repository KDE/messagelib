/*
  Copyright (C) 2016 Sandro Knauß <sknauss@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef __MIMETREEPARSER_MESSAGEPARTRENDERER_IF_H__
#define __MIMETREEPARSER_MESSAGEPARTRENDERER_IF_H__

#include "mimetreeparser_export.h"

#include <QSharedPointer>

namespace MimeTreeParser {
namespace Interface {
/**
* Interface for rendering messageparts to html.
* @author Andras Mantia <sknauss@kde.org>
*/
class MIMETREEPARSER_EXPORT MessagePartRenderer
{
public:
    typedef QSharedPointer<MessagePartRenderer> Ptr;

    virtual ~MessagePartRenderer();

    virtual QString html() const = 0;
};
}
}
#endif

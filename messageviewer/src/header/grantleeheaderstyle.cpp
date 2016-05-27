/*
  Copyright (c) 2013-2016 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License, version 2, as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "grantleeheaderstyle.h"
#include "header/grantleeheaderformatter.h"
#include "header/headerstrategy.h"
#include "grantleetheme/grantleetheme.h"

#include <kmime/kmime_message.h>

using namespace MessageViewer;
class MessageViewer::GrantleeHeaderStylePrivate
{
public:
    GrantleeHeaderStylePrivate()
        : mGrantleeFormatter(Q_NULLPTR),
          mShowMailAction(true)
    {
        mGrantleeFormatter = new GrantleeHeaderFormatter;
    }
    ~GrantleeHeaderStylePrivate()
    {
        delete mGrantleeFormatter;
    }

    GrantleeHeaderFormatter *mGrantleeFormatter;
    bool mShowMailAction;
};

GrantleeHeaderStyle::GrantleeHeaderStyle()
    : HeaderStyle(),
      d(new MessageViewer::GrantleeHeaderStylePrivate)
{
}

GrantleeHeaderStyle::~GrantleeHeaderStyle()
{
    delete d;
}

const char *GrantleeHeaderStyle::name() const
{
    return "grantlee";
}



QString GrantleeHeaderStyle::format(KMime::Message *message) const
{
    if (!message) {
        return QString();
    }

    GrantleeHeaderFormatter::GrantleeHeaderFormatterSettings settings;
    settings.isPrinting = isPrinting();
    settings.theme = theme();
    settings.style = this;
    settings.message = message;
    settings.showMailAction = d->mShowMailAction;
    return d->mGrantleeFormatter->toHtml(settings);
}

bool GrantleeHeaderStyle::hasAttachmentQuickList() const
{
    return true;
}

bool GrantleeHeaderStyle::showMailAction() const
{
    return d->mShowMailAction;
}

void GrantleeHeaderStyle::setShowMailAction(bool showAction)
{
    d->mShowMailAction = showAction;
}

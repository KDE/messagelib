/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGEVIEWER_DEFAULTRENDERER_MESSAGEPARTRENDERER_H
#define MESSAGEVIEWER_DEFAULTRENDERER_MESSAGEPARTRENDERER_H

#include "../messagepartrendererbase.h"

#include <QSharedPointer>

namespace MessageViewer {
class MessagePartRenderer : public MessagePartRendererBase
{
public:
    MessagePartRenderer();
    ~MessagePartRenderer() override;
    bool render(const MimeTreeParser::MessagePartPtr &msgPart, HtmlWriter *htmlWriter, RenderContext *context) const override;
};
}

#endif

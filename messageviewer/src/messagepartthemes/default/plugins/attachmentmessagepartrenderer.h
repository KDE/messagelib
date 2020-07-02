/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MESSAGEVIEWER_DEFAULTRENDERER_ATTACHMENTMESSAGEPARTRENDERER_H
#define MESSAGEVIEWER_DEFAULTRENDERER_ATTACHMENTMESSAGEPARTRENDERER_H

#include "../messagepartrendererbase.h"

namespace MessageViewer {
class AttachmentMessagePartRenderer : public MessagePartRendererBase
{
public:
    AttachmentMessagePartRenderer();
    ~AttachmentMessagePartRenderer() override;
    bool render(const MimeTreeParser::MessagePartPtr &msgPart, HtmlWriter *htmlWriter, RenderContext *context) const override;
};
}

#endif

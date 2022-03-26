/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "../messagepartrendererbase.h"

namespace MessageViewer
{
class TextMessagePartRenderer : public MessagePartRendererBase
{
public:
    TextMessagePartRenderer();
    ~TextMessagePartRenderer() override;
    bool render(const MimeTreeParser::MessagePartPtr &msgPart, HtmlWriter *htmlWriter, RenderContext *context) const override;
};
}

/*
   SPDX-FileCopyrightText: 2016 Sandro Knauß <sknauss@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QString>

namespace MessageViewer
{
class HtmlWriter;
class RenderContext;
}

void quotedHTML(const QString &s, MessageViewer::RenderContext *context, MessageViewer::HtmlWriter *htmlWriter);

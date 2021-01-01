/*
   SPDX-FileCopyrightText: 2019-2021 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "messageviewerrenderer.h"
#include "viewer/csshelper.h"
using namespace MessageViewer;
MessageViewerRenderer::MessageViewerRenderer()
{
}

MessageViewerRenderer::~MessageViewerRenderer()
{
    delete mCSSHelper;
}

CSSHelper *MessageViewerRenderer::cssHelper() const
{
    return mCSSHelper;
}

QWidget *MessageViewerRenderer::currentWidget() const
{
    return mCurrentWidget;
}

void MessageViewerRenderer::setCurrentWidget(QWidget *currentWidget)
{
    mCurrentWidget = currentWidget;
}

void MessageViewerRenderer::recreateCssHelper()
{
    delete mCSSHelper;
    mCSSHelper = new CSSHelper(mCurrentWidget);
}

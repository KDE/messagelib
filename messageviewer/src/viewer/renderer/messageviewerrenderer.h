/*
   SPDX-FileCopyrightText: 2019-2024 Laurent Montel <montel@kde.org>

   Code based on ARHParser.jsm from dkim_verifier (Copyright (c) Philippe Lieser)
   (This software is licensed under the terms of the MIT License.)

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include "messageviewer_private_export.h"
#include <QWidget>
namespace MessageViewer
{
class CSSHelper;
class MESSAGEVIEWER_TESTS_EXPORT MessageViewerRenderer
{
public:
    MessageViewerRenderer();
    ~MessageViewerRenderer();

    CSSHelper *cssHelper() const;

    QWidget *currentWidget() const;
    void setCurrentWidget(QWidget *currentWidget);

    void recreateCssHelper();

private:
    CSSHelper *mCSSHelper = nullptr;
    QWidget *mCurrentWidget = nullptr;
};
}

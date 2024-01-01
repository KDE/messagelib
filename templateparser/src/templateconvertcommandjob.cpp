/*
   SPDX-FileCopyrightText: 2018-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "templateconvertcommandjob.h"

using namespace TemplateParser;

TemplateConvertCommandJob::TemplateConvertCommandJob(QObject *parent)
    : QObject(parent)
{
}

TemplateConvertCommandJob::~TemplateConvertCommandJob() = default;

QString TemplateConvertCommandJob::convertText()
{
    return {};
}

QString TemplateConvertCommandJob::currentText() const
{
    return mCurrentText;
}

void TemplateConvertCommandJob::setCurrentText(const QString &currentText)
{
    mCurrentText = currentText;
}

KMime::Message::Ptr TemplateConvertCommandJob::originalMessage() const
{
    return mOriginalMessage;
}

void TemplateConvertCommandJob::setOriginalMessage(const KMime::Message::Ptr &originalMessage)
{
    mOriginalMessage = originalMessage;
}

#include "moc_templateconvertcommandjob.cpp"

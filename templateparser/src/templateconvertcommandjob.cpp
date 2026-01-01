/*
   SPDX-FileCopyrightText: 2018-2026 Laurent Montel <montel@kde.org>

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

std::shared_ptr<KMime::Message> TemplateConvertCommandJob::originalMessage() const
{
    return mOriginalMessage;
}

void TemplateConvertCommandJob::setOriginalMessage(const std::shared_ptr<KMime::Message> &originalMessage)
{
    mOriginalMessage = originalMessage;
}

#include "moc_templateconvertcommandjob.cpp"

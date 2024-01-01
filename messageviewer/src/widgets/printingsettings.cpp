/*
  SPDX-FileCopyrightText: 2013-2024 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "printingsettings.h"
#include "settings/messageviewersettings.h"
#include "ui_printingsettings.h"
#include <PimCommon/ConfigureImmutableWidgetUtils>
using namespace PimCommon::ConfigureImmutableWidgetUtils;

using namespace MessageViewer;

class MessageViewer::PrintingSettingsPrivate
{
public:
    PrintingSettingsPrivate()
        : mPrintingUi(new Ui_PrintingSettings)
    {
    }

    ~PrintingSettingsPrivate()
    {
        delete mPrintingUi;
    }

    Ui_PrintingSettings *const mPrintingUi;
};

PrintingSettings::PrintingSettings(QWidget *parent)
    : QWidget(parent)
    , d(new MessageViewer::PrintingSettingsPrivate)
{
    d->mPrintingUi->setupUi(this);
    connect(d->mPrintingUi->mPrintEmptySelectedText, &QCheckBox::toggled, this, &PrintingSettings::changed);
    connect(d->mPrintingUi->respectExpandCollapseSettings, &QCheckBox::toggled, this, &PrintingSettings::changed);
    connect(d->mPrintingUi->printBackgroundColorAndImages, &QCheckBox::toggled, this, &PrintingSettings::changed);
    connect(d->mPrintingUi->alwaysShowEncryptionSignatureDetail, &QCheckBox::toggled, this, &PrintingSettings::changed);
}

PrintingSettings::~PrintingSettings() = default;

void PrintingSettings::save()
{
    saveCheckBox(d->mPrintingUi->mPrintEmptySelectedText, MessageViewer::MessageViewerSettings::self()->printSelectedTextItem());
    saveCheckBox(d->mPrintingUi->respectExpandCollapseSettings, MessageViewer::MessageViewerSettings::self()->respectExpandCollapseSettingsItem());
    saveCheckBox(d->mPrintingUi->printBackgroundColorAndImages, MessageViewer::MessageViewerSettings::self()->printBackgroundColorImagesItem());
    saveCheckBox(d->mPrintingUi->alwaysShowEncryptionSignatureDetail, MessageViewer::MessageViewerSettings::self()->alwaysShowEncryptionSignatureDetailsItem());
}

void PrintingSettings::doLoadFromGlobalSettings()
{
    loadWidget(d->mPrintingUi->mPrintEmptySelectedText, MessageViewer::MessageViewerSettings::self()->printSelectedTextItem());
    loadWidget(d->mPrintingUi->respectExpandCollapseSettings, MessageViewer::MessageViewerSettings::self()->respectExpandCollapseSettingsItem());
    loadWidget(d->mPrintingUi->printBackgroundColorAndImages, MessageViewer::MessageViewerSettings::self()->printBackgroundColorImagesItem());
    loadWidget(d->mPrintingUi->alwaysShowEncryptionSignatureDetail, MessageViewer::MessageViewerSettings::self()->alwaysShowEncryptionSignatureDetailsItem());
}

void PrintingSettings::doResetToDefaultsOther()
{
    const bool bUseDefaults = MessageViewer::MessageViewerSettings::self()->useDefaults(true);
    doLoadFromGlobalSettings();
    MessageViewer::MessageViewerSettings::self()->useDefaults(bUseDefaults);
}

#include "moc_printingsettings.cpp"

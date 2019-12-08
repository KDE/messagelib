/*
  Copyright (c) 2013-2019 Montel Laurent <montel@kde.org>

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "printingsettings.h"
#include "ui_printingsettings.h"
#include "settings/messageviewersettings.h"
#include "PimCommon/ConfigureImmutableWidgetUtils"
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

    Ui_PrintingSettings *mPrintingUi = nullptr;
};

PrintingSettings::PrintingSettings(QWidget *parent)
    : QWidget(parent)
    , d(new MessageViewer::PrintingSettingsPrivate)
{
    d->mPrintingUi->setupUi(this);
    connect(d->mPrintingUi->mPrintEmptySelectedText, &QCheckBox::toggled, this,
            &PrintingSettings::changed);
    connect(d->mPrintingUi->respectExpandCollapseSettings, &QCheckBox::toggled, this,
            &PrintingSettings::changed);
    connect(d->mPrintingUi->printBackgroundColorAndImages, &QCheckBox::toggled, this,
            &PrintingSettings::changed);
    connect(d->mPrintingUi->alwaysShowEncryptionSignatureDetail, &QCheckBox::toggled, this,
            &PrintingSettings::changed);
}

PrintingSettings::~PrintingSettings()
{
    delete d;
}

void PrintingSettings::save()
{
    saveCheckBox(d->mPrintingUi->mPrintEmptySelectedText,
                 MessageViewer::MessageViewerSettings::self()->printSelectedTextItem());
    saveCheckBox(d->mPrintingUi->respectExpandCollapseSettings,
                 MessageViewer::MessageViewerSettings::self()->respectExpandCollapseSettingsItem());
    saveCheckBox(d->mPrintingUi->printBackgroundColorAndImages,
                 MessageViewer::MessageViewerSettings::self()->printBackgroundColorImagesItem());
    saveCheckBox(d->mPrintingUi->alwaysShowEncryptionSignatureDetail,
                 MessageViewer::MessageViewerSettings::self()->alwaysShowEncryptionSignatureDetailsItem());
}

void PrintingSettings::doLoadFromGlobalSettings()
{
    loadWidget(d->mPrintingUi->mPrintEmptySelectedText,
               MessageViewer::MessageViewerSettings::self()->printSelectedTextItem());
    loadWidget(d->mPrintingUi->respectExpandCollapseSettings,
               MessageViewer::MessageViewerSettings::self()->respectExpandCollapseSettingsItem());
    loadWidget(d->mPrintingUi->printBackgroundColorAndImages,
               MessageViewer::MessageViewerSettings::self()->printBackgroundColorImagesItem());
    loadWidget(d->mPrintingUi->alwaysShowEncryptionSignatureDetail,
               MessageViewer::MessageViewerSettings::self()->alwaysShowEncryptionSignatureDetailsItem());
}

void PrintingSettings::doResetToDefaultsOther()
{
    const bool bUseDefaults = MessageViewer::MessageViewerSettings::self()->useDefaults(true);
    doLoadFromGlobalSettings();
    MessageViewer::MessageViewerSettings::self()->useDefaults(bUseDefaults);
}

/*
  SPDX-FileCopyrightText: 2009 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.net
  SPDX-FileCopyrightText: 2009 Andras Mantia <andras@kdab.net>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "configurewidget.h"
#include "messageviewer_debug.h"
#include "settings/messageviewersettings.h"
#include "ui_settings.h"
#include <MimeTreeParser/NodeHelper>

#include "MessageCore/MessageCoreSettings"

#include <KLocalizedString>

using namespace MessageViewer;

class MessageViewer::ConfigureWidgetPrivate
{
public:
    ConfigureWidgetPrivate() = default;

    ~ConfigureWidgetPrivate()
    {
        delete mSettingsUi;
        mSettingsUi = nullptr;
    }

    Ui_Settings *mSettingsUi = nullptr;
};

ConfigureWidget::ConfigureWidget(QWidget *parent)
    : QWidget(parent)
    , d(new MessageViewer::ConfigureWidgetPrivate)
{
    d->mSettingsUi = new Ui_Settings;
    d->mSettingsUi->setupUi(this);
    layout()->setContentsMargins({});

    QStringList encodings = MimeTreeParser::NodeHelper::supportedEncodings();
    encodings.prepend(i18n("Auto"));
    d->mSettingsUi->overrideCharacterEncoding->addItems(encodings);
    d->mSettingsUi->overrideCharacterEncoding->setCurrentIndex(0);

    d->mSettingsUi->overrideCharacterEncoding->setWhatsThis(MessageCore::MessageCoreSettings::self()->overrideCharacterEncodingItem()->whatsThis());
    d->mSettingsUi->kcfg_ShrinkQuotes->setWhatsThis(MessageViewer::MessageViewerSettings::self()->shrinkQuotesItem()->whatsThis());
    d->mSettingsUi->kcfg_ShowExpandQuotesMark->setWhatsThis(MessageViewer::MessageViewerSettings::self()->showExpandQuotesMarkItem()->whatsThis());
    connect(d->mSettingsUi->overrideCharacterEncoding, &QComboBox::currentIndexChanged, this, &ConfigureWidget::settingsChanged);
}

ConfigureWidget::~ConfigureWidget() = default;

void ConfigureWidget::readConfig()
{
    readCurrentOverrideCodec();
    d->mSettingsUi->kcfg_CollapseQuoteLevelSpin->setEnabled(MessageViewer::MessageViewerSettings::self()->showExpandQuotesMark());
}

void ConfigureWidget::writeConfig()
{
    MessageCore::MessageCoreSettings::self()->setOverrideCharacterEncoding(
        d->mSettingsUi->overrideCharacterEncoding->currentIndex() == 0
            ? QString()
            : MimeTreeParser::NodeHelper::encodingForName(d->mSettingsUi->overrideCharacterEncoding->currentText()));
}

void ConfigureWidget::readCurrentOverrideCodec()
{
    const QString &currentOverrideEncoding = MessageCore::MessageCoreSettings::self()->overrideCharacterEncoding();
    if (currentOverrideEncoding.isEmpty()) {
        d->mSettingsUi->overrideCharacterEncoding->setCurrentIndex(0);
        return;
    }
    QStringList encodings = MimeTreeParser::NodeHelper::supportedEncodings();
    encodings.prepend(i18n("Auto"));
    QStringList::ConstIterator it(encodings.constBegin());
    const QStringList::ConstIterator end(encodings.constEnd());
    int i = 0;
    for (; it != end; ++it) {
        if (MimeTreeParser::NodeHelper::encodingForName(*it) == currentOverrideEncoding) {
            d->mSettingsUi->overrideCharacterEncoding->setCurrentIndex(i);
            break;
        }
        ++i;
    }
    if (i == encodings.size()) {
        // the current value of overrideCharacterEncoding is an unknown encoding => reset to Auto
        qCWarning(MESSAGEVIEWER_LOG) << "Unknown override character encoding" << currentOverrideEncoding << ". Resetting to Auto.";
        d->mSettingsUi->overrideCharacterEncoding->setCurrentIndex(0);
        MessageCore::MessageCoreSettings::self()->setOverrideCharacterEncoding(QString());
    }
}

#include "moc_configurewidget.cpp"

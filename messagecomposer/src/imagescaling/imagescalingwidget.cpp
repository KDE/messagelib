/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "imagescalingwidget.h"
using namespace Qt::Literals::StringLiterals;

#include "settings/messagecomposersettings.h"
#include "ui_imagescalingwidget.h"
#include <KLineEditEventHandler>
#include <KLocalizedString>
#include <KMessageBox>

#include <QButtonGroup>
#include <QComboBox>
#include <QImageWriter>
#include <QList>
#include <QWhatsThis>

using namespace MessageComposer;
class MessageComposer::ImageScalingWidgetPrivate
{
public:
    ImageScalingWidgetPrivate()
        : ui(new Ui::ImageScalingWidget)
    {
    }

    ~ImageScalingWidgetPrivate()
    {
        delete ui;
    }

    Ui::ImageScalingWidget *const ui;
    QButtonGroup *mSourceFilenameFilterGroup = nullptr;
    QButtonGroup *mRecipientFilterGroup = nullptr;
    bool mWasChanged = false;
};

ImageScalingWidget::ImageScalingWidget(QWidget *parent)
    : QWidget(parent)
    , d(new MessageComposer::ImageScalingWidgetPrivate)
{
    d->ui->setupUi(this);
    initComboBox(d->ui->CBMaximumWidth);
    initComboBox(d->ui->CBMaximumHeight);
    initComboBox(d->ui->CBMinimumWidth);
    initComboBox(d->ui->CBMinimumHeight);

    initWriteImageFormat();
    KLineEditEventHandler::catchReturnKey(d->ui->pattern);
    KLineEditEventHandler::catchReturnKey(d->ui->renameResizedImagePattern);
    KLineEditEventHandler::catchReturnKey(d->ui->resizeEmailsPattern);
    KLineEditEventHandler::catchReturnKey(d->ui->doNotResizePattern);

    connect(d->ui->enabledAutoResize, &QCheckBox::clicked, this, &ImageScalingWidget::changed);
    connect(d->ui->KeepImageRatio, &QCheckBox::clicked, this, &ImageScalingWidget::changed);
    connect(d->ui->AskBeforeResizing, &QCheckBox::clicked, this, &ImageScalingWidget::changed);
    connect(d->ui->EnlargeImageToMinimum, &QCheckBox::clicked, this, &ImageScalingWidget::changed);
    connect(d->ui->ReduceImageToMaximum, &QCheckBox::clicked, this, &ImageScalingWidget::changed);
    connect(d->ui->customMaximumWidth, &QSpinBox::valueChanged, this, &ImageScalingWidget::changed);
    connect(d->ui->customMaximumHeight, &QSpinBox::valueChanged, this, &ImageScalingWidget::changed);
    connect(d->ui->customMinimumWidth, &QSpinBox::valueChanged, this, &ImageScalingWidget::changed);
    connect(d->ui->customMinimumHeight, &QSpinBox::valueChanged, this, &ImageScalingWidget::changed);
    connect(d->ui->skipImageSizeLower, &QCheckBox::clicked, this, &ImageScalingWidget::changed);
    connect(d->ui->imageSize, &QSpinBox::valueChanged, this, &ImageScalingWidget::changed);
    connect(d->ui->pattern, &QLineEdit::textChanged, this, &ImageScalingWidget::changed);
    connect(d->ui->CBMaximumWidth, &QComboBox::currentIndexChanged, this, &ImageScalingWidget::slotComboboxChanged);
    connect(d->ui->CBMaximumHeight, &QComboBox::currentIndexChanged, this, &ImageScalingWidget::slotComboboxChanged);
    connect(d->ui->CBMinimumWidth, &QComboBox::currentIndexChanged, this, &ImageScalingWidget::slotComboboxChanged);
    connect(d->ui->CBMinimumHeight, &QComboBox::currentIndexChanged, this, &ImageScalingWidget::slotComboboxChanged);
    connect(d->ui->WriteToImageFormat, &QComboBox::activated, this, &ImageScalingWidget::changed);
    connect(d->ui->renameResizedImage, &QCheckBox::clicked, this, &ImageScalingWidget::changed);
    connect(d->ui->renameResizedImage, &QCheckBox::clicked, d->ui->renameResizedImagePattern, &QLineEdit::setEnabled);
    connect(d->ui->renameResizedImagePattern, &QLineEdit::textChanged, this, &ImageScalingWidget::changed);

    connect(d->ui->resizeEmailsPattern, &QLineEdit::textChanged, this, &ImageScalingWidget::changed);
    connect(d->ui->doNotResizePattern, &QLineEdit::textChanged, this, &ImageScalingWidget::changed);
    connect(d->ui->resizeImageWithFormatsType, &MessageComposer::ImageScalingSelectFormat::textChanged, this, &ImageScalingWidget::changed);
    connect(d->ui->resizeImageWithFormats, &QCheckBox::clicked, this, &ImageScalingWidget::changed);
    connect(d->ui->resizeImageWithFormats, &QCheckBox::clicked, d->ui->resizeImageWithFormatsType, &MessageComposer::ImageScalingSelectFormat::setEnabled);
    d->ui->resizeImageWithFormatsType->setEnabled(false);

    d->ui->pattern->setEnabled(false);
    d->mSourceFilenameFilterGroup = new QButtonGroup(d->ui->filterSourceGroupBox);
    connect(d->mSourceFilenameFilterGroup, &QButtonGroup::buttonClicked, this, &ImageScalingWidget::slotSourceFilterAbstractButtonClicked);
    d->mSourceFilenameFilterGroup->addButton(d->ui->notFilterFilename, MessageComposer::MessageComposerSettings::EnumFilterSourceType::NoFilter);
    d->mSourceFilenameFilterGroup->addButton(d->ui->includeFilesWithPattern,
                                             MessageComposer::MessageComposerSettings::EnumFilterSourceType::IncludeFilesWithPattern);
    d->mSourceFilenameFilterGroup->addButton(d->ui->excludeFilesWithPattern,
                                             MessageComposer::MessageComposerSettings::EnumFilterSourceType::ExcludeFilesWithPattern);

    d->mRecipientFilterGroup = new QButtonGroup(d->ui->tab_4);
    connect(d->mRecipientFilterGroup, &QButtonGroup::buttonClicked, this, &ImageScalingWidget::slotRecipientFilterAbstractClicked);
    d->ui->doNotResizePattern->setEnabled(false);
    d->ui->resizeEmailsPattern->setEnabled(false);
    d->mRecipientFilterGroup->addButton(d->ui->doNotFilterRecipients, MessageComposer::MessageComposerSettings::EnumFilterRecipientType::NoFilter);
    d->mRecipientFilterGroup->addButton(d->ui->resizeEachEmails,
                                        MessageComposer::MessageComposerSettings::EnumFilterRecipientType::ResizeEachEmailsContainsPattern);
    d->mRecipientFilterGroup->addButton(d->ui->resizeOneEmails,
                                        MessageComposer::MessageComposerSettings::EnumFilterRecipientType::ResizeOneEmailContainsPattern);
    d->mRecipientFilterGroup->addButton(d->ui->doNotResizeEachEmails,
                                        MessageComposer::MessageComposerSettings::EnumFilterRecipientType::DontResizeEachEmailsContainsPattern);
    d->mRecipientFilterGroup->addButton(d->ui->doNotResizeOneEmails,
                                        MessageComposer::MessageComposerSettings::EnumFilterRecipientType::DontResizeOneEmailContainsPattern);

    d->ui->help->setText(i18n("<a href=\"whatsthis\">How does this work?</a>"));
    connect(d->ui->help, &QLabel::linkActivated, this, &ImageScalingWidget::slotHelpLinkClicked);
    d->ui->help->setContextMenuPolicy(Qt::NoContextMenu);
}

ImageScalingWidget::~ImageScalingWidget() = default;

void ImageScalingWidget::slotHelpLinkClicked(const QString &)
{
    const QString help = i18n(
        "<qt>"
        "<p>Here you can define image filename. "
        "You can use:</p>"
        "<ul>"
        "<li>%t set current time</li>"
        "<li>%d set current date</li>"
        "<li>%n original filename</li>"
        "<li>%e original extension</li>"
        "<li>%x new extension</li>"
        "</ul>"
        "</qt>");

    QWhatsThis::showText(QCursor::pos(), help);
}

void ImageScalingWidget::slotSourceFilterAbstractButtonClicked(QAbstractButton *button)
{
    if (button) {
        const int buttonId = d->mRecipientFilterGroup->id(button);
        slotSourceFilterClicked(buttonId);
    }
}

void ImageScalingWidget::slotSourceFilterClicked(int button)
{
    d->ui->pattern->setEnabled(button != 0);
    Q_EMIT changed();
}

void ImageScalingWidget::slotRecipientFilterClicked(int button)
{
    d->ui->resizeEmailsPattern->setEnabled((button == MessageComposer::MessageComposerSettings::EnumFilterRecipientType::ResizeEachEmailsContainsPattern)
                                           || (button == MessageComposer::MessageComposerSettings::EnumFilterRecipientType::ResizeOneEmailContainsPattern));
    d->ui->doNotResizePattern->setEnabled((button == MessageComposer::MessageComposerSettings::EnumFilterRecipientType::DontResizeEachEmailsContainsPattern)
                                          || (button == MessageComposer::MessageComposerSettings::EnumFilterRecipientType::DontResizeOneEmailContainsPattern));
    Q_EMIT changed();
}

void ImageScalingWidget::slotRecipientFilterAbstractClicked(QAbstractButton *button)
{
    if (button) {
        const int buttonId = d->mRecipientFilterGroup->id(button);
        slotRecipientFilterClicked(buttonId);
    }
}

void ImageScalingWidget::slotComboboxChanged(int index)
{
    auto combo = qobject_cast<QComboBox *>(sender());
    if (combo) {
        const bool isCustom = combo->itemData(index) == -1;
        if (combo == d->ui->CBMaximumWidth) {
            d->ui->customMaximumWidth->setEnabled(isCustom);
        } else if (combo == d->ui->CBMaximumHeight) {
            d->ui->customMaximumHeight->setEnabled(isCustom);
        } else if (combo == d->ui->CBMinimumWidth) {
            d->ui->customMinimumWidth->setEnabled(isCustom);
        } else if (combo == d->ui->CBMinimumHeight) {
            d->ui->customMinimumHeight->setEnabled(isCustom);
        }
        Q_EMIT changed();
    }
}

void ImageScalingWidget::initComboBox(QComboBox *combo)
{
    const QList<int> size = {240, 320, 512, 640, 800, 1024, 1600, 2048};
    for (int val : size) {
        combo->addItem(u"%1 px"_s.arg(QString::number(val)), val);
    }
    combo->addItem(i18n("Custom"), -1);
}

void ImageScalingWidget::initWriteImageFormat()
{
    /* Too many format :)
    QList<QByteArray> listWriteFormat = QImageWriter::supportedImageFormats();
    Q_FOREACH(const QByteArray& format, listWriteFormat) {
        d->ui->WriteToImageFormat->addItem(QString::fromLatin1(format));
    }
    */
    // known by several mailer.
    d->ui->WriteToImageFormat->addItem(u"JPG"_s);
    d->ui->WriteToImageFormat->addItem(u"PNG"_s);
}

void ImageScalingWidget::updateSettings()
{
    d->ui->enabledAutoResize->setChecked(MessageComposer::MessageComposerSettings::self()->autoResizeImageEnabled());
    d->ui->KeepImageRatio->setChecked(MessageComposer::MessageComposerSettings::self()->keepImageRatio());
    d->ui->AskBeforeResizing->setChecked(MessageComposer::MessageComposerSettings::self()->askBeforeResizing());
    d->ui->EnlargeImageToMinimum->setChecked(MessageComposer::MessageComposerSettings::self()->enlargeImageToMinimum());
    d->ui->ReduceImageToMaximum->setChecked(MessageComposer::MessageComposerSettings::self()->reduceImageToMaximum());
    d->ui->skipImageSizeLower->setChecked(MessageComposer::MessageComposerSettings::self()->skipImageLowerSizeEnabled());
    d->ui->imageSize->setValue(MessageComposer::MessageComposerSettings::self()->skipImageLowerSize());

    d->ui->customMaximumWidth->setValue(MessageComposer::MessageComposerSettings::self()->customMaximumWidth());
    d->ui->customMaximumHeight->setValue(MessageComposer::MessageComposerSettings::self()->customMaximumHeight());
    d->ui->customMinimumWidth->setValue(MessageComposer::MessageComposerSettings::self()->customMinimumWidth());
    d->ui->customMinimumHeight->setValue(MessageComposer::MessageComposerSettings::self()->customMinimumHeight());

    int index = qMax(0, d->ui->CBMaximumWidth->findData(MessageComposer::MessageComposerSettings::self()->maximumWidth()));
    d->ui->CBMaximumWidth->setCurrentIndex(index);
    d->ui->customMaximumWidth->setEnabled(d->ui->CBMaximumWidth->itemData(index) == -1);

    index = qMax(0, d->ui->CBMaximumHeight->findData(MessageComposer::MessageComposerSettings::self()->maximumHeight()));
    d->ui->CBMaximumHeight->setCurrentIndex(index);
    d->ui->customMaximumHeight->setEnabled(d->ui->CBMaximumHeight->itemData(index) == -1);

    index = qMax(0, d->ui->CBMinimumWidth->findData(MessageComposer::MessageComposerSettings::self()->minimumWidth()));
    d->ui->CBMinimumWidth->setCurrentIndex(index);
    d->ui->customMinimumWidth->setEnabled(d->ui->CBMinimumWidth->itemData(index) == -1);

    index = qMax(0, d->ui->CBMinimumHeight->findData(MessageComposer::MessageComposerSettings::self()->minimumHeight()));
    d->ui->CBMinimumHeight->setCurrentIndex(index);
    d->ui->customMinimumHeight->setEnabled(d->ui->CBMinimumHeight->itemData(index) == -1);

    index = d->ui->WriteToImageFormat->findData(MessageComposer::MessageComposerSettings::self()->writeFormat());
    if (index == -1) {
        d->ui->WriteToImageFormat->setCurrentIndex(0);
    } else {
        d->ui->WriteToImageFormat->setCurrentIndex(index);
    }
    d->ui->pattern->setText(MessageComposer::MessageComposerSettings::self()->filterSourcePattern());

    d->ui->renameResizedImage->setChecked(MessageComposer::MessageComposerSettings::self()->renameResizedImages());

    d->ui->renameResizedImagePattern->setText(MessageComposer::MessageComposerSettings::self()->renameResizedImagesPattern());
    d->ui->renameResizedImagePattern->setEnabled(d->ui->renameResizedImage->isChecked());

    d->ui->doNotResizePattern->setText(MessageComposer::MessageComposerSettings::self()->doNotResizeEmailsPattern());
    d->ui->resizeEmailsPattern->setText(MessageComposer::MessageComposerSettings::self()->resizeEmailsPattern());

    d->ui->resizeImageWithFormats->setChecked(MessageComposer::MessageComposerSettings::self()->resizeImagesWithFormats());
    d->ui->resizeImageWithFormatsType->setFormat(MessageComposer::MessageComposerSettings::self()->resizeImagesWithFormatsType());
    d->ui->resizeImageWithFormatsType->setEnabled(d->ui->resizeImageWithFormats->isChecked());

    updateFilterSourceTypeSettings();
    updateEmailsFilterTypeSettings();
}

void ImageScalingWidget::loadConfig()
{
    updateSettings();
    d->mWasChanged = false;
}

void ImageScalingWidget::updateFilterSourceTypeSettings()
{
    switch (MessageComposer::MessageComposerSettings::self()->filterSourceType()) {
    case MessageComposer::MessageComposerSettings::EnumFilterSourceType::NoFilter:
        d->ui->notFilterFilename->setChecked(true);
        d->ui->pattern->setEnabled(false);
        break;
    case MessageComposer::MessageComposerSettings::EnumFilterSourceType::IncludeFilesWithPattern:
        d->ui->includeFilesWithPattern->setChecked(true);
        d->ui->pattern->setEnabled(true);
        break;
    case MessageComposer::MessageComposerSettings::EnumFilterSourceType::ExcludeFilesWithPattern:
        d->ui->excludeFilesWithPattern->setChecked(true);
        d->ui->pattern->setEnabled(true);
        break;
    }
}

void ImageScalingWidget::updateEmailsFilterTypeSettings()
{
    d->ui->doNotResizePattern->setEnabled(false);
    d->ui->resizeEmailsPattern->setEnabled(false);

    switch (MessageComposer::MessageComposerSettings::self()->filterRecipientType()) {
    case MessageComposer::MessageComposerSettings::EnumFilterRecipientType::NoFilter:
        d->ui->doNotFilterRecipients->setChecked(true);
        break;
    case MessageComposer::MessageComposerSettings::EnumFilterRecipientType::ResizeEachEmailsContainsPattern:
        d->ui->resizeEachEmails->setChecked(true);
        d->ui->resizeEmailsPattern->setEnabled(true);
        break;
    case MessageComposer::MessageComposerSettings::EnumFilterRecipientType::ResizeOneEmailContainsPattern:
        d->ui->resizeOneEmails->setChecked(true);
        d->ui->resizeEmailsPattern->setEnabled(true);
        break;
    case MessageComposer::MessageComposerSettings::EnumFilterRecipientType::DontResizeEachEmailsContainsPattern:
        d->ui->doNotResizeEachEmails->setChecked(true);
        d->ui->doNotResizePattern->setEnabled(true);
        break;
    case MessageComposer::MessageComposerSettings::EnumFilterRecipientType::DontResizeOneEmailContainsPattern:
        d->ui->doNotResizeOneEmails->setChecked(true);
        d->ui->doNotResizePattern->setEnabled(false);
        break;
    }
}

void ImageScalingWidget::writeConfig()
{
    if (d->ui->EnlargeImageToMinimum->isChecked() && d->ui->ReduceImageToMaximum->isChecked()) {
        if ((d->ui->customMinimumWidth->value() >= d->ui->customMaximumWidth->value())
            || (d->ui->customMinimumHeight->value() >= d->ui->customMaximumHeight->value())) {
            KMessageBox::error(this, i18n("Please verify minimum and maximum values."), i18nc("@title:window", "Error in minimum Maximum value"));
            return;
        }
    }
    MessageComposer::MessageComposerSettings::self()->setAutoResizeImageEnabled(d->ui->enabledAutoResize->isChecked());
    MessageComposer::MessageComposerSettings::self()->setKeepImageRatio(d->ui->KeepImageRatio->isChecked());
    MessageComposer::MessageComposerSettings::self()->setAskBeforeResizing(d->ui->AskBeforeResizing->isChecked());
    MessageComposer::MessageComposerSettings::self()->setEnlargeImageToMinimum(d->ui->EnlargeImageToMinimum->isChecked());
    MessageComposer::MessageComposerSettings::self()->setReduceImageToMaximum(d->ui->ReduceImageToMaximum->isChecked());

    MessageComposer::MessageComposerSettings::self()->setCustomMaximumWidth(d->ui->customMaximumWidth->value());
    MessageComposer::MessageComposerSettings::self()->setCustomMaximumHeight(d->ui->customMaximumHeight->value());
    MessageComposer::MessageComposerSettings::self()->setCustomMinimumWidth(d->ui->customMinimumWidth->value());
    MessageComposer::MessageComposerSettings::self()->setCustomMinimumHeight(d->ui->customMinimumHeight->value());

    MessageComposer::MessageComposerSettings::self()->setMaximumWidth(d->ui->CBMaximumWidth->itemData(d->ui->CBMaximumWidth->currentIndex()).toInt());
    MessageComposer::MessageComposerSettings::self()->setMaximumHeight(d->ui->CBMaximumHeight->itemData(d->ui->CBMaximumHeight->currentIndex()).toInt());
    MessageComposer::MessageComposerSettings::self()->setMinimumWidth(d->ui->CBMinimumWidth->itemData(d->ui->CBMinimumWidth->currentIndex()).toInt());
    MessageComposer::MessageComposerSettings::self()->setMinimumHeight(d->ui->CBMinimumHeight->itemData(d->ui->CBMinimumHeight->currentIndex()).toInt());

    MessageComposer::MessageComposerSettings::self()->setWriteFormat(d->ui->WriteToImageFormat->currentText());
    MessageComposer::MessageComposerSettings::self()->setSkipImageLowerSizeEnabled(d->ui->skipImageSizeLower->isChecked());
    MessageComposer::MessageComposerSettings::self()->setSkipImageLowerSize(d->ui->imageSize->value());

    MessageComposer::MessageComposerSettings::self()->setFilterSourcePattern(d->ui->pattern->text());

    MessageComposer::MessageComposerSettings::self()->setFilterSourceType(d->mSourceFilenameFilterGroup->checkedId());

    MessageComposer::MessageComposerSettings::self()->setRenameResizedImages(d->ui->renameResizedImage->isChecked());

    MessageComposer::MessageComposerSettings::self()->setRenameResizedImagesPattern(d->ui->renameResizedImagePattern->text());

    MessageComposer::MessageComposerSettings::self()->setDoNotResizeEmailsPattern(d->ui->doNotResizePattern->text());
    MessageComposer::MessageComposerSettings::self()->setResizeEmailsPattern(d->ui->resizeEmailsPattern->text());
    MessageComposer::MessageComposerSettings::self()->setFilterRecipientType(d->mRecipientFilterGroup->checkedId());

    MessageComposer::MessageComposerSettings::self()->setResizeImagesWithFormats(d->ui->resizeImageWithFormats->isChecked());
    MessageComposer::MessageComposerSettings::self()->setResizeImagesWithFormatsType(d->ui->resizeImageWithFormatsType->format());
    d->mWasChanged = false;
}

void ImageScalingWidget::resetToDefault()
{
    const bool bUseDefaults = MessageComposer::MessageComposerSettings::self()->useDefaults(true);
    updateSettings();
    MessageComposer::MessageComposerSettings::self()->useDefaults(bUseDefaults);
}

#include "moc_imagescalingwidget.cpp"

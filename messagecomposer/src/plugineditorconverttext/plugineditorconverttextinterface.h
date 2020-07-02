/*
   SPDX-FileCopyrightText: 2018-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PLUGINEDITORCONVERTTEXTINTERFACE_H
#define PLUGINEDITORCONVERTTEXTINTERFACE_H

#include <QObject>
#include "messagecomposer_export.h"
#include <MessageComposer/PluginActionType>
#include <KMime/Message>

namespace KPIMTextEdit {
class RichTextComposer;
}
class KActionCollection;
namespace MessageComposer {
class TextPart;
class PluginEditorConvertTextInterfacePrivate;
class PluginEditorConverterInitialData;
class PluginEditorConverterBeforeConvertingData;
class PluginEditorConvertText;
/**
 * @brief The PluginEditorConvertTextInterface class
 * @author Laurent Montel <montel@kde.org>
 */
class MESSAGECOMPOSER_EXPORT PluginEditorConvertTextInterface : public QObject
{
    Q_OBJECT
public:
    explicit PluginEditorConvertTextInterface(QObject *parent = nullptr);
    ~PluginEditorConvertTextInterface();

    enum class ConvertTextStatus {
        NotConverted,
        Converted,
        Error
    };

    virtual bool reformatText();

    virtual PluginEditorConvertTextInterface::ConvertTextStatus convertTextToFormat(MessageComposer::TextPart *textPart) = 0;

    void setParentWidget(QWidget *parent);
    Q_REQUIRED_RESULT QWidget *parentWidget() const;

    Q_REQUIRED_RESULT KPIMTextEdit::RichTextComposer *richTextEditor() const;
    void setRichTextEditor(KPIMTextEdit::RichTextComposer *richTextEditor);

    void setActionType(const QVector<PluginActionType> &type);
    void addActionType(const PluginActionType &type);
    Q_REQUIRED_RESULT QVector<PluginActionType> actionTypes() const;

    virtual void createAction(KActionCollection *ac);

    virtual void setInitialData(const PluginEditorConverterInitialData &data);
    Q_REQUIRED_RESULT PluginEditorConverterInitialData initialData() const;

    virtual void setBeforeConvertingData(const PluginEditorConverterBeforeConvertingData &data);
    Q_REQUIRED_RESULT PluginEditorConverterBeforeConvertingData beforeConvertingData() const;

    virtual void enableDisablePluginActions(bool richText);

    void setStatusBarWidget(QWidget *w);

    QWidget *statusBarWidget() const;

    void setPlugin(PluginEditorConvertText *plugin);
    Q_REQUIRED_RESULT PluginEditorConvertText *plugin() const;

public Q_SLOTS:
    virtual void reloadConfig();

Q_SIGNALS:
    void textReformated();

private:
    PluginEditorConvertTextInterfacePrivate *const d;
};
}

#endif // PLUGINEDITORCONVERTTEXTINTERFACE_H

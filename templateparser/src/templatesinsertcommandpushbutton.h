/*
 * SPDX-FileCopyrightText: 2006 Dmitry Morozhnikov <dmiceman@mail.ru>
 * SPDX-FileCopyrightText: 2018-2021 Laurent Montel <montel@kde.org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#pragma once

#include "templateparser_export.h"
#include <QPushButton>
#include <TemplateParser/TemplatesCommandMenu>

namespace TemplateParser
{
/**
 * @brief The TemplatesInsertCommandPushButton class
 * @author Laurent Montel <montel@kde.org>
 */
class TEMPLATEPARSER_EXPORT TemplatesInsertCommandPushButton : public QPushButton
{
    Q_OBJECT
public:
    explicit TemplatesInsertCommandPushButton(QWidget *parent, const QString &name = QString());
    ~TemplatesInsertCommandPushButton() override;

    Q_REQUIRED_RESULT TemplatesCommandMenu::MenuTypes type() const;
    void setType(TemplatesCommandMenu::MenuTypes type);

Q_SIGNALS:
    void insertCommand(const QString &cmd, int adjustCursor = 0);

private:
    TemplatesCommandMenu *const mMenuCommand;
};
}


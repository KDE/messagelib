#include "mailinglist.h"

#include <kconfig.h>
#include <kconfiggroup.h>
#include <QUrl>
#include "messagecore_debug.h"

#include <QSharedData>
#include <QStringList>

using namespace MessageCore;

typedef QString (*MagicDetectorFunc)(const KMime::Message::Ptr &, QByteArray &, QString &);

/* Sender: (owner-([^@]+)|([^@+]-owner)@ */
static QString check_sender(const KMime::Message::Ptr &message, QByteArray &headerName, QString &headerValue)
{
    QString header = message->sender()->asUnicodeString();

    if (header.isEmpty()) {
        return QString();
    }

    if (header.left(6) == QLatin1String("owner-")) {
        headerName = "Sender";
        headerValue = header;
        header = header.mid(6, header.indexOf(QLatin1Char('@')) - 6);
    } else {
        const int index = header.indexOf(QLatin1String("-owner@ "));
        if (index == -1) {
            return QString();
        }

        header.truncate(index);
        headerName = "Sender";
        headerValue = header;
    }

    return header;
}

/* X-BeenThere: ([^@]+) */
static QString check_x_beenthere(const KMime::Message::Ptr &message, QByteArray &headerName, QString &headerValue)
{
    QString header;
    if (auto hrd = message->headerByType("X-BeenThere")) {
        header = hrd->asUnicodeString();
    }
    if (header.isNull() || header.indexOf(QLatin1Char('@')) == -1) {
        return QString();
    }

    headerName = "X-BeenThere";
    headerValue = header;
    header.truncate(header.indexOf(QLatin1Char('@')));

    return header;
}

/* Delivered-To:: <([^@]+) */
static QString check_delivered_to(const KMime::Message::Ptr &message, QByteArray &headerName, QString &headerValue)
{
    QString header;
    if (auto hrd = message->headerByType("Delivered-To")) {
        header = hrd->asUnicodeString();
    }
    if (header.isNull()
        || header.left(13) != QLatin1String("mailing list")
        || header.indexOf(QLatin1Char('@')) == -1) {
        return QString();
    }

    headerName = "Delivered-To";
    headerValue = header;

    return header.mid(13, header.indexOf(QLatin1Char('@')) - 13);
}

/* X-Mailing-List: <?([^@]+) */
static QString check_x_mailing_list(const KMime::Message::Ptr &message, QByteArray &headerName, QString &headerValue)
{
    QString header;
    if (auto hrd = message->headerByType("X-Mailing-List")) {
        header = hrd->asUnicodeString();
    }
    if (header.isEmpty()) {
        return QString();
    }

    if (header.indexOf(QLatin1Char('@')) < 1) {
        return QString();
    }

    headerName = "X-Mailing-List";
    headerValue = header;
    if (header[0] == QLatin1Char('<')) {
        header = header.mid(1, header.indexOf(QLatin1Char('@')) - 1);
    } else {
        header.truncate(header.indexOf(QLatin1Char('@')));
    }

    return header;
}

/* List-Id: [^<]* <([^.]+) */
static QString check_list_id(const KMime::Message::Ptr &message, QByteArray &headerName, QString &headerValue)
{
    QString header;
    if (auto hrd = message->headerByType("List-Id")) {
        header = hrd->asUnicodeString();
    }
    if (header.isEmpty()) {
        return QString();
    }

    const int leftAnglePos = header.indexOf(QLatin1Char('<'));
    if (leftAnglePos < 0) {
        return QString();
    }

    const int firstDotPos = header.indexOf(QLatin1Char('.'), leftAnglePos);
    if (firstDotPos < 0) {
        return QString();
    }

    headerName = "List-Id";
    headerValue = header.mid(leftAnglePos);
    header = header.mid(leftAnglePos + 1, firstDotPos - leftAnglePos - 1);

    return header;
}

/* List-Post: <mailto:[^< ]*>) */
static QString check_list_post(const KMime::Message::Ptr &message, QByteArray &headerName, QString &headerValue)
{
    QString header;
    if (auto hrd = message->headerByType("List-Post")) {
        header = hrd->asUnicodeString();
    }
    if (header.isEmpty()) {
        return QString();
    }

    int leftAnglePos = header.indexOf(QLatin1String("<mailto:"));
    if (leftAnglePos < 0) {
        return QString();
    }

    headerName = "List-Post";
    headerValue = header;
    header = header.mid(leftAnglePos + 8, header.length());
    header.truncate(header.indexOf(QLatin1Char('@')));

    return header;
}

/* Mailing-List: list ([^@]+) */
static QString check_mailing_list(const KMime::Message::Ptr &message, QByteArray &headerName, QString &headerValue)
{
    QString header;
    if (auto hrd = message->headerByType("Mailing-List")) {
        header = hrd->asUnicodeString();
    }
    if (header.isEmpty()) {
        return QString();
    }

    if (header.left(5) != QLatin1String("list ")
        || header.indexOf(QLatin1Char('@')) < 5) {
        return QString();
    }

    headerName = "Mailing-List";
    headerValue = header;
    header = header.mid(5, header.indexOf(QLatin1Char('@')) - 5);

    return header;
}

/* X-Loop: ([^@]+) */
static QString check_x_loop(const KMime::Message::Ptr &message, QByteArray &headerName, QString &headerValue)
{
    QString header;
    if (auto hrd = message->headerByType("X-Loop")) {
        header = hrd->asUnicodeString();
    }
    if (header.isEmpty()) {
        return QString();
    }

    const int indexOfHeader(header.indexOf(QLatin1Char('@')));
    if (indexOfHeader < 2) {
        return QString();
    }

    headerName = "X-Loop";
    headerValue = header;
    header.truncate(indexOfHeader);

    return header;
}

/* X-ML-Name: (.+) */
static QString check_x_ml_name(const KMime::Message::Ptr &message, QByteArray &headerName, QString &headerValue)
{
    QString header;
    if (auto hrd = message->headerByType("X-ML-Name")) {
        header = hrd->asUnicodeString();
    }
    if (header.isEmpty()) {
        return QString();
    }

    headerName = "X-ML-Name";
    headerValue = header;
    header.truncate(header.indexOf(QLatin1Char('@')));

    return header;
}

static const MagicDetectorFunc magic_detector[] = {
    check_list_id,
    check_list_post,
    check_sender,
    check_x_mailing_list,
    check_mailing_list,
    check_delivered_to,
    check_x_beenthere,
    check_x_loop,
    check_x_ml_name
};

static const int num_detectors = sizeof(magic_detector) / sizeof(magic_detector[0]);

static QStringList headerToAddress(const QString &header)
{
    QStringList addresses;
    int start = 0;

    if (header.isEmpty()) {
        return addresses;
    }

    while ((start = header.indexOf(QLatin1Char('<'), start)) != -1) {
        int end = 0;
        if ((end = header.indexOf(QLatin1Char('>'), ++start)) == -1) {
            qCWarning(MESSAGECORE_LOG) << "Serious mailing list header parsing error!";
            return addresses;
        }

        addresses.append(header.mid(start, end - start));
    }

    return addresses;
}

class Q_DECL_HIDDEN MessageCore::MailingList::Private : public QSharedData
{
public:
    Private()
        : mFeatures(None)
        , mHandler(KMail)
    {
    }

    Private(const Private &other)
        : QSharedData(other)
    {
        mFeatures = other.mFeatures;
        mHandler = other.mHandler;
        mPostUrls = other.mPostUrls;
        mSubscribeUrls = other.mSubscribeUrls;
        mUnsubscribeUrls = other.mUnsubscribeUrls;
        mHelpUrls = other.mHelpUrls;
        mArchiveUrls = other.mArchiveUrls;
        mOwnerUrls = other.mOwnerUrls;
        mArchivedAtUrls = other.mArchivedAtUrls;
        mId = other.mId;
    }

    Features mFeatures;
    Handler mHandler;
    QList<QUrl> mPostUrls;
    QList<QUrl> mSubscribeUrls;
    QList<QUrl> mUnsubscribeUrls;
    QList<QUrl> mHelpUrls;
    QList<QUrl> mArchiveUrls;
    QList<QUrl> mOwnerUrls;
    QList<QUrl> mArchivedAtUrls;
    QString mId;
};

MailingList MailingList::detect(const KMime::Message::Ptr &message)
{
    MailingList mailingList;

    if (auto hrd = message->headerByType("List-Post")) {
        mailingList.setPostUrls(QUrl::fromStringList(headerToAddress(hrd->asUnicodeString())));
    }

    if (auto hrd = message->headerByType("List-Help")) {
        mailingList.setHelpUrls(QUrl::fromStringList(headerToAddress(hrd->asUnicodeString())));
    }

    if (auto hrd = message->headerByType("List-Subscribe")) {
        mailingList.setSubscribeUrls(QUrl::fromStringList(headerToAddress(hrd->asUnicodeString())));
    }

    if (auto hrd = message->headerByType("List-Unsubscribe")) {
        mailingList.setUnsubscribeUrls(QUrl::fromStringList(headerToAddress(hrd->asUnicodeString())));
    }

    if (auto hrd = message->headerByType("List-Archive")) {
        mailingList.setArchiveUrls(QUrl::fromStringList(headerToAddress(hrd->asUnicodeString())));
    }

    if (auto hrd = message->headerByType("List-Owner")) {
        mailingList.setOwnerUrls(QUrl::fromStringList(headerToAddress(hrd->asUnicodeString())));
    }

    if (auto hrd = message->headerByType("Archived-At")) {
        mailingList.setArchivedAtUrls(QUrl::fromStringList(headerToAddress(hrd->asUnicodeString())));
    }

    if (auto hrd = message->headerByType("List-Id")) {
        mailingList.setId(hrd->asUnicodeString());
    }

    return mailingList;
}

QString MailingList::name(const KMime::Message::Ptr &message, QByteArray &headerName, QString &headerValue)
{
    QString mailingList;
    headerName = QByteArray();
    headerValue.clear();

    if (!message) {
        return QString();
    }

    for (int i = 0; i < num_detectors; ++i) {
        mailingList = magic_detector[i](message, headerName, headerValue);
        if (!mailingList.isNull()) {
            return mailingList;
        }
    }

    return QString();
}

MailingList::MailingList()
    : d(new Private)
{
}

MailingList::MailingList(const MailingList &other)
    : d(other.d)
{
}

MailingList &MailingList::operator=(const MailingList &other)
{
    if (this != &other) {
        d = other.d;
    }

    return *this;
}

bool MailingList::operator==(const MailingList &other) const
{
    return other.features() == d->mFeatures
           && other.handler() == d->mHandler
           && other.postUrls() == d->mPostUrls
           && other.subscribeUrls() == d->mSubscribeUrls
           && other.unsubscribeUrls() == d->mUnsubscribeUrls
           && other.helpUrls() == d->mHelpUrls
           && other.archiveUrls() == d->mArchiveUrls
           && other.ownerUrls() == d->mOwnerUrls
           && other.archivedAtUrls() == d->mArchivedAtUrls
           && other.id() == d->mId;
}

MailingList::~MailingList()
{
}

MailingList::Features MailingList::features() const
{
    return d->mFeatures;
}

void MailingList::setHandler(MailingList::Handler handler)
{
    d->mHandler = handler;
}

MailingList::Handler MailingList::handler() const
{
    return d->mHandler;
}

void MailingList::setPostUrls(const QList<QUrl> &urls)
{
    d->mFeatures |= Post;

    if (urls.empty()) {
        d->mFeatures ^= Post;
    }

    d->mPostUrls = urls;
}

QList<QUrl> MailingList::postUrls() const
{
    return d->mPostUrls;
}

void MailingList::setSubscribeUrls(const QList<QUrl> &urls)
{
    d->mFeatures |= Subscribe;

    if (urls.empty()) {
        d->mFeatures ^= Subscribe;
    }

    d->mSubscribeUrls = urls;
}

QList<QUrl> MailingList::subscribeUrls() const
{
    return d->mSubscribeUrls;
}

void MailingList::setUnsubscribeUrls(const QList<QUrl> &urls)
{
    d->mFeatures |= Unsubscribe;

    if (urls.empty()) {
        d->mFeatures ^= Unsubscribe;
    }

    d->mUnsubscribeUrls = urls;
}

QList<QUrl> MailingList::unsubscribeUrls() const
{
    return d->mUnsubscribeUrls;
}

void MailingList::setHelpUrls(const QList<QUrl> &urls)
{
    d->mFeatures |= Help;

    if (urls.empty()) {
        d->mFeatures ^= Help;
    }

    d->mHelpUrls = urls;
}

QList<QUrl> MailingList::helpUrls() const
{
    return d->mHelpUrls;
}

void MailingList::setArchiveUrls(const QList<QUrl> &urls)
{
    d->mFeatures |= Archive;

    if (urls.empty()) {
        d->mFeatures ^= Archive;
    }

    d->mArchiveUrls = urls;
}

QList<QUrl> MailingList::archiveUrls() const
{
    return d->mArchiveUrls;
}

void MailingList::setOwnerUrls(const QList<QUrl> &urls)
{
    d->mFeatures |= Owner;

    if (urls.empty()) {
        d->mFeatures ^= Owner;
    }

    d->mOwnerUrls = urls;
}

QList<QUrl> MailingList::ownerUrls() const
{
    return d->mOwnerUrls;
}

void MailingList::setArchivedAtUrls(const QList<QUrl> &urls)
{
    d->mFeatures |= ArchivedAt;

    if (urls.isEmpty()) {
        d->mFeatures ^= ArchivedAt;
    }

    d->mArchivedAtUrls = urls;
}

QList<QUrl> MailingList::archivedAtUrls() const
{
    return d->mArchivedAtUrls;
}

void MailingList::setId(const QString &id)
{
    d->mFeatures |= Id;

    if (id.isEmpty()) {
        d->mFeatures ^= Id;
    }

    d->mId = id;
}

QString MailingList::id() const
{
    return d->mId;
}

void MailingList::writeConfig(KConfigGroup &group) const
{
    group.writeEntry("MailingListFeatures", static_cast<int>(d->mFeatures));
    group.writeEntry("MailingListHandler", static_cast<int>(d->mHandler));
    group.writeEntry("MailingListId", d->mId);
    QStringList lst = QUrl::toStringList(d->mPostUrls);
    if (!lst.isEmpty()) {
        group.writeEntry("MailingListPostingAddress", lst);
    } else {
        group.deleteEntry("MailingListPostingAddress");
    }

    lst = QUrl::toStringList(d->mSubscribeUrls);
    if (!lst.isEmpty()) {
        group.writeEntry("MailingListSubscribeAddress", lst);
    } else {
        group.deleteEntry("MailingListSubscribeAddress");
    }

    lst = QUrl::toStringList(d->mUnsubscribeUrls);
    if (!lst.isEmpty()) {
        group.writeEntry("MailingListUnsubscribeAddress", lst);
    } else {
        group.deleteEntry("MailingListUnsubscribeAddress");
    }

    lst = QUrl::toStringList(d->mArchiveUrls);
    if (!lst.isEmpty()) {
        group.writeEntry("MailingListArchiveAddress", lst);
    } else {
        group.deleteEntry("MailingListArchiveAddress");
    }

    lst = QUrl::toStringList(d->mOwnerUrls);
    if (!lst.isEmpty()) {
        group.writeEntry("MailingListOwnerAddress", lst);
    } else {
        group.deleteEntry("MailingListOwnerAddress");
    }

    lst = QUrl::toStringList(d->mHelpUrls);
    if (!lst.isEmpty()) {
        group.writeEntry("MailingListHelpAddress", lst);
    } else {
        group.deleteEntry("MailingListHelpAddress");
    }

    /* Note: mArchivedAtUrl deliberately not saved here as it refers to a single
    * instance of a message rather than an element of a general mailing list.
    * http://reviewboard.kde.org/r/1768/#review2783
    */
}

void MailingList::readConfig(const KConfigGroup &group)
{
    d->mFeatures = static_cast<MailingList::Features>(group.readEntry("MailingListFeatures", 0));
    d->mHandler = static_cast<MailingList::Handler>(group.readEntry("MailingListHandler",
                                                                    static_cast<int>(MailingList::KMail)));
    d->mId = group.readEntry("MailingListId");
    d->mPostUrls = QUrl::fromStringList(group.readEntry("MailingListPostingAddress", QStringList()));
    d->mSubscribeUrls = QUrl::fromStringList(group.readEntry("MailingListSubscribeAddress", QStringList()));
    d->mUnsubscribeUrls = QUrl::fromStringList(group.readEntry("MailingListUnsubscribeAddress", QStringList()));
    d->mArchiveUrls = QUrl::fromStringList(group.readEntry("MailingListArchiveAddress", QStringList()));
    d->mOwnerUrls = QUrl::fromStringList(group.readEntry("MailingListOwnerAddress", QStringList()));
    d->mHelpUrls = QUrl::fromStringList(group.readEntry("MailingListHelpAddress", QStringList()));
}

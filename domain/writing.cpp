#include "domain/writing.hpp"

#include <utility>

namespace {

QString normalizeLineBreaks(QString value)
{
    value.replace("\r\n", "\n");
    value.replace('\r', '\n');
    return value;
}

} // namespace

namespace Domain {

Writing::Writing(
    QString id,
    QString title,
    QString authorId,
    QString genre,
    int publicationYear,
    QString description)
    : m_id(std::move(id))
    , m_title(std::move(title))
    , m_authorId(std::move(authorId))
    , m_genre(std::move(genre))
    , m_publicationYear(publicationYear)
    , m_description(std::move(description))
{
}

const QString &Writing::id() const
{
    return m_id;
}

const QString &Writing::title() const
{
    return m_title;
}

const QString &Writing::authorId() const
{
    return m_authorId;
}

const QString &Writing::genre() const
{
    return m_genre;
}

int Writing::publicationYear() const
{
    return m_publicationYear;
}

const QString &Writing::description() const
{
    return m_description;
}

Writing Writing::normalized() const
{
    return Writing(
        m_id.trimmed(),
        m_title.trimmed(),
        m_authorId.trimmed(),
        m_genre.trimmed(),
        m_publicationYear,
        normalizeLineBreaks(m_description).trimmed());
}

Writing Writing::withId(QString id) const
{
    return Writing(std::move(id), m_title, m_authorId, m_genre, m_publicationYear, m_description);
}

QString Writing::validationError() const
{
    if (m_title.trimmed().isEmpty()) {
        return QStringLiteral("Название произведения не может быть пустым.");
    }

    if (m_authorId.trimmed().isEmpty()) {
        return QStringLiteral("Для произведения нужно выбрать автора.");
    }

    if (m_publicationYear < 0 || m_publicationYear > 2100) {
        return QStringLiteral("Год издания должен быть в диапазоне от 0 до 2100.");
    }

    return {};
}

} // namespace Domain

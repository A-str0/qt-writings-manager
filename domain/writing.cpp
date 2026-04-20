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
    QString title, QString author, QString genre, int publicationYear, QString description)
    : m_title(std::move(title))
    , m_author(std::move(author))
    , m_genre(std::move(genre))
    , m_publicationYear(publicationYear)
    , m_description(std::move(description))
{
}

const QString &Writing::title() const
{
    return m_title;
}

const QString &Writing::author() const
{
    return m_author;
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
        m_title.trimmed(),
        m_author.trimmed(),
        m_genre.trimmed(),
        m_publicationYear,
        normalizeLineBreaks(m_description).trimmed());
}

QString Writing::validationError() const
{
    if (m_title.trimmed().isEmpty()) {
        return QStringLiteral("Название произведения не может быть пустым.");
    }

    if (m_author.trimmed().isEmpty()) {
        return QStringLiteral("Поле автора обязательно для заполнения.");
    }

    if (m_publicationYear < 0 || m_publicationYear > 2100) {
        return QStringLiteral("Год издания должен быть в диапазоне от 0 до 2100.");
    }

    return {};
}

} // namespace Domain

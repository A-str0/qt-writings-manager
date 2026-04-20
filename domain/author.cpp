#include "domain/author.hpp"

#include <utility>

namespace Domain {

Author::Author(QString id, QString name, int birthYear, QString country)
    : m_id(std::move(id))
    , m_name(std::move(name))
    , m_birthYear(birthYear)
    , m_country(std::move(country))
{
}

const QString &Author::id() const
{
    return m_id;
}

const QString &Author::name() const
{
    return m_name;
}

int Author::birthYear() const
{
    return m_birthYear;
}

const QString &Author::country() const
{
    return m_country;
}

Author Author::normalized() const
{
    return Author(m_id.trimmed(), m_name.trimmed(), m_birthYear, m_country.trimmed());
}

Author Author::withId(QString id) const
{
    return Author(std::move(id), m_name, m_birthYear, m_country);
}

QString Author::validationError() const
{
    if (m_name.trimmed().isEmpty()) {
        return QStringLiteral("Имя автора не может быть пустым.");
    }

    if (m_birthYear < 0 || m_birthYear > 2100) {
        return QStringLiteral("Год рождения автора должен быть в диапазоне от 0 до 2100.");
    }

    return {};
}

} // namespace Domain

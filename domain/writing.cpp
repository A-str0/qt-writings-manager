#include "domain/writing.hpp"

#include <utility>

namespace Domain {

Writing::Writing(QUuid id, QUuid authorId, QString title, QString description)
    : m_id(std::move(id))
    , m_authorId(std::move(authorId))
    , m_title(std::move(title))
    , m_description(std::move(description))
{}

QString Writing::validationError() const
{
    if (m_title.trimmed().isEmpty()) {
        return QStringLiteral("название не может быть пустым");
    }

    if (m_authorId.isNull()) {
        return QStringLiteral("необходимо выбрать автора");
    }

    return {};
}

} // namespace Domain

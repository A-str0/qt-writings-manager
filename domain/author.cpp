#include "domain/author.hpp"

#include <utility>

namespace Domain {

Author::Author(QUuid id, QString name)
    : m_id(std::move(id))
    , m_name(std::move(name))
{}

QString Author::validationError() const
{
    if (m_name.trimmed().isEmpty()) {
        return QStringLiteral("имя автора не может быть пустым");
    }

    return {};
}

} // namespace Domain

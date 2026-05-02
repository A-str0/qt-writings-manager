#include "domain/author.hpp"

#include <utility>

namespace Domain {

Author::Author(QUuid id, QString name)
    : id(std::move(id))
    , name(std::move(name))
{}

QString Author::validationError() const
{
    if (name.trimmed().isEmpty()) {
        return QStringLiteral("имя автора не может быть пустым");
    }

    return {};
}

} // namespace Domain

#include "domain/writing.hpp"

#include <utility>

namespace Domain {

Writing::Writing(QUuid id, QUuid authorId, QString title, QString description)
    : id(std::move(id))
    , title(std::move(title))
    , authorId(std::move(authorId))
    , description(std::move(description))
{}

QString Writing::validationError() const
{
    if (title.trimmed().isEmpty()) {
        return QStringLiteral("название не может быть пустым");
    }

    if (authorId.isNull()) {
        return QStringLiteral("необходимо выбрать автора");
    }

    return {};
}

} // namespace Domain

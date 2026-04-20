#ifndef APPLICATION_AUTHORS_FILE_STORAGE_HPP
#define APPLICATION_AUTHORS_FILE_STORAGE_HPP

#include <QList>
#include <QString>

#include "application/operation_result.hpp"

namespace Application {

class AuthorsFileStorage
{
public:
    virtual ~AuthorsFileStorage() = default;

    virtual OperationResult save(
        const QString &filePath, const QList<Domain::Author> &authors) const = 0;
    virtual LoadAuthorsResult load(const QString &filePath) const = 0;
};

} // namespace Application

#endif // APPLICATION_AUTHORS_FILE_STORAGE_HPP

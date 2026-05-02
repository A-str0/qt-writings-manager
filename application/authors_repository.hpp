#ifndef APPLICATION_AUTHORS_REPOSITORY_HPP
#define APPLICATION_AUTHORS_REPOSITORY_HPP

#include <QList>
#include <QString>
#include <QUuid>

#include "application/operation_result.hpp"

namespace Application {

class AuthorsRepository
{
public:
    virtual ~AuthorsRepository() = default;

    virtual QList<Domain::Author> findAll() const = 0;
    virtual bool existsById(const QUuid &authorId) const = 0;

    virtual OperationResult add(const Domain::Author &author) = 0;
    virtual OperationResult update(const QUuid &authorId, const Domain::Author &author) = 0;
    virtual OperationResult remove(const QUuid &authorId) = 0;
    virtual OperationResult replaceAll(const QList<Domain::Author> &authors) = 0;
};

} // namespace Application

#endif // APPLICATION_AUTHORS_REPOSITORY_HPP

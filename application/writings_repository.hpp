#ifndef APPLICATION_WRITINGS_REPOSITORY_HPP
#define APPLICATION_WRITINGS_REPOSITORY_HPP

#include <QList>
#include <QString>
#include <QUuid>

#include "application/operation_result.hpp"

namespace Application {

class WritingsRepository
{
public:
    virtual ~WritingsRepository() = default;

    virtual QList<Domain::Writing> findAll() const = 0;
    virtual bool hasForAuthor(const QUuid &authorId) const = 0;

    virtual OperationResult add(const Domain::Writing &writing) = 0;
    virtual OperationResult update(const QUuid &writingId, const Domain::Writing &writing) = 0;
    virtual OperationResult remove(const QUuid &writingId) = 0;
    virtual OperationResult replaceAll(const QList<Domain::Writing> &writings) = 0;
};

} // namespace Application

#endif // APPLICATION_WRITINGS_REPOSITORY_HPP

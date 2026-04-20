#ifndef APPLICATION_WRITINGS_REPOSITORY_HPP
#define APPLICATION_WRITINGS_REPOSITORY_HPP

#include <QList>
#include <QString>

#include "application/operation_result.hpp"

namespace Application {

class WritingsRepository
{
public:
    virtual ~WritingsRepository() = default;

    virtual QList<Domain::Writing> findAll() const = 0;
    virtual bool hasForAuthor(const QString &authorId) const = 0;

    virtual OperationResult add(const Domain::Writing &writing) = 0;
    virtual OperationResult update(const QString &writingId, const Domain::Writing &writing) = 0;
    virtual OperationResult remove(const QString &writingId) = 0;
    virtual OperationResult replaceAll(const QList<Domain::Writing> &writings) = 0;
};

} // namespace Application

#endif // APPLICATION_WRITINGS_REPOSITORY_HPP

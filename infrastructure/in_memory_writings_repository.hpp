#ifndef INFRASTRUCTURE_IN_MEMORY_WRITINGS_REPOSITORY_HPP
#define INFRASTRUCTURE_IN_MEMORY_WRITINGS_REPOSITORY_HPP

#include <QMap>
#include <QUuid>

#include "application/writings_repository.hpp"

namespace Infrastructure {

class InMemoryWritingsRepository final : public Application::WritingsRepository
{
public:
    QList<Domain::Writing> findAll() const override;
    bool hasForAuthor(const QUuid &authorId) const override;

    Application::OperationResult add(const Domain::Writing &writing) override;
    Application::OperationResult update(const QUuid &writingId,
                                        const Domain::Writing &writing) override;
    Application::OperationResult remove(const QUuid &writingId) override;
    Application::OperationResult replaceAll(const QList<Domain::Writing> &writings) override;

private:
    QMap<QUuid, Domain::Writing> _writings;
};

} // namespace Infrastructure

#endif // INFRASTRUCTURE_IN_MEMORY_WRITINGS_REPOSITORY_HPP

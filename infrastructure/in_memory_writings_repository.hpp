#ifndef INFRASTRUCTURE_IN_MEMORY_WRITINGS_REPOSITORY_HPP
#define INFRASTRUCTURE_IN_MEMORY_WRITINGS_REPOSITORY_HPP

#include <QMap>
#include <QString>

#include "application/writings_repository.hpp"

namespace Infrastructure {

class InMemoryWritingsRepository final : public Application::WritingsRepository
{
public:
    QList<Domain::Writing> findAll() const override;
    bool hasForAuthor(const QString &authorId) const override;

    Application::OperationResult add(const Domain::Writing &writing) override;
    Application::OperationResult update(
        const QString &writingId, const Domain::Writing &writing) override;
    Application::OperationResult remove(const QString &writingId) override;
    Application::OperationResult replaceAll(const QList<Domain::Writing> &writings) override;

private:
    static QString keyForId(const QString &id);

    QMap<QString, Domain::Writing> m_writings;
};

} // namespace Infrastructure

#endif // INFRASTRUCTURE_IN_MEMORY_WRITINGS_REPOSITORY_HPP

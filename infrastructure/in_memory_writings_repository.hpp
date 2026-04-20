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
    Application::OperationResult add(const Domain::Writing &writing) override;
    Application::OperationResult update(
        const QString &currentTitle, const Domain::Writing &writing) override;
    Application::OperationResult remove(const QString &title) override;
    Application::OperationResult replaceAll(const QList<Domain::Writing> &writings) override;

private:
    static QString keyForTitle(const QString &title);

    QMap<QString, Domain::Writing> m_writings;
};

} // namespace Infrastructure

#endif // INFRASTRUCTURE_IN_MEMORY_WRITINGS_REPOSITORY_HPP

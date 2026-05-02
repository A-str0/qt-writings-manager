#ifndef INFRASTRUCTURE_IN_MEMORY_AUTHORS_REPOSITORY_HPP
#define INFRASTRUCTURE_IN_MEMORY_AUTHORS_REPOSITORY_HPP

#include <QMap>
#include <QUuid>

#include "application/authors_repository.hpp"

namespace Infrastructure {

class InMemoryAuthorsRepository final : public Application::AuthorsRepository
{
public:
    QList<Domain::Author> findAll() const override;
    bool existsById(const QUuid &authorId) const override;

    Application::OperationResult add(const Domain::Author &author) override;
    Application::OperationResult update(const QUuid &authorId,
                                        const Domain::Author &author) override;
    Application::OperationResult remove(const QUuid &authorId) override;
    Application::OperationResult replaceAll(const QList<Domain::Author> &authors) override;

private:
    QMap<QUuid, Domain::Author> _authors;
};

} // namespace Infrastructure

#endif // INFRASTRUCTURE_IN_MEMORY_AUTHORS_REPOSITORY_HPP

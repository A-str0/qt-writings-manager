#ifndef INFRASTRUCTURE_IN_MEMORY_AUTHORS_REPOSITORY_HPP
#define INFRASTRUCTURE_IN_MEMORY_AUTHORS_REPOSITORY_HPP

#include <QMap>
#include <QString>

#include "application/authors_repository.hpp"

namespace Infrastructure {

class InMemoryAuthorsRepository final : public Application::AuthorsRepository
{
public:
    QList<Domain::Author> findAll() const override;
    bool existsById(const QString &authorId) const override;

    Application::OperationResult add(const Domain::Author &author) override;
    Application::OperationResult update(const QString &authorId, const Domain::Author &author) override;
    Application::OperationResult remove(const QString &authorId) override;
    Application::OperationResult replaceAll(const QList<Domain::Author> &authors) override;

private:
    static QString keyForId(const QString &id);
    static QString keyForName(const QString &name);
    bool authorNameExists(const QString &name, const QString &exceptAuthorId = {}) const;

    QMap<QString, Domain::Author> m_authors;
};

} // namespace Infrastructure

#endif // INFRASTRUCTURE_IN_MEMORY_AUTHORS_REPOSITORY_HPP

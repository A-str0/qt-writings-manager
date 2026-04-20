#ifndef APPLICATION_AUTHORS_SERVICE_HPP
#define APPLICATION_AUTHORS_SERVICE_HPP

#include <QList>
#include <QString>

#include "application/authors_file_storage.hpp"
#include "application/authors_repository.hpp"
#include "application/writings_repository.hpp"

namespace Application {

class AuthorsService
{
public:
    AuthorsService(
        AuthorsRepository &authorsRepository,
        WritingsRepository &writingsRepository,
        AuthorsFileStorage &fileStorage);

    QList<Domain::Author> listAuthors() const;

    OperationResult addAuthor(const Domain::Author &author);
    OperationResult updateAuthor(const QString &authorId, const Domain::Author &author);
    OperationResult removeAuthor(const QString &authorId);

    OperationResult saveToFile(const QString &filePath) const;
    OperationResult loadFromFile(const QString &filePath);

private:
    AuthorsRepository &m_authorsRepository;
    WritingsRepository &m_writingsRepository;
    AuthorsFileStorage &m_fileStorage;
};

} // namespace Application

#endif // APPLICATION_AUTHORS_SERVICE_HPP

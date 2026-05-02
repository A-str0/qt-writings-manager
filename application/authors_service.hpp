#ifndef APPLICATION_AUTHORS_SERVICE_HPP
#define APPLICATION_AUTHORS_SERVICE_HPP

#include <QList>
#include <QString>
#include <QUuid>

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

    OperationResult addAuthor(Domain::Author author);
    OperationResult updateAuthor(const QUuid &authorId, Domain::Author author);
    OperationResult removeAuthor(const QUuid &authorId);

    OperationResult saveToFile(const QString &filePath) const;
    OperationResult loadFromFile(const QString &filePath);

private:
    AuthorsRepository &m_authorsRepository;
    WritingsRepository &m_writingsRepository;
    AuthorsFileStorage &m_fileStorage;
};

} // namespace Application

#endif // APPLICATION_AUTHORS_SERVICE_HPP

#ifndef APPLICATION_WRITINGS_SERVICE_HPP
#define APPLICATION_WRITINGS_SERVICE_HPP

#include <QList>
#include <QString>

#include "application/authors_repository.hpp"
#include "application/writings_file_storage.hpp"
#include "application/writings_repository.hpp"

namespace Application {

class WritingsService
{
public:
    WritingsService(
        WritingsRepository &writingsRepository,
        AuthorsRepository &authorsRepository,
        WritingsFileStorage &fileStorage);

    QList<Domain::Writing> listWritings() const;

    OperationResult addWriting(const Domain::Writing &writing);
    OperationResult updateWriting(const QString &writingId, const Domain::Writing &writing);
    OperationResult removeWriting(const QString &writingId);

    OperationResult saveToFile(const QString &filePath) const;
    OperationResult loadFromFile(const QString &filePath);

private:
    WritingsRepository &m_writingsRepository;
    AuthorsRepository &m_authorsRepository;
    WritingsFileStorage &m_fileStorage;
};

} // namespace Application

#endif // APPLICATION_WRITINGS_SERVICE_HPP

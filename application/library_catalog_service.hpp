#ifndef APPLICATION_LIBRARY_CATALOG_SERVICE_HPP
#define APPLICATION_LIBRARY_CATALOG_SERVICE_HPP

#include <QList>
#include <QString>

#include "application/writing_file_storage.hpp"
#include "application/writings_repository.hpp"

namespace Application {

class LibraryCatalogService
{
public:
    LibraryCatalogService(WritingsRepository &repository, WritingFileStorage &fileStorage);

    QList<Domain::Writing> listWritings() const;
    OperationResult addWriting(const Domain::Writing &writing);
    OperationResult updateWriting(const QString &currentTitle, const Domain::Writing &writing);
    OperationResult removeWriting(const QString &title);
    OperationResult saveToFile(const QString &filePath) const;
    OperationResult loadFromFile(const QString &filePath);

private:
    WritingsRepository &m_repository;
    WritingFileStorage &m_fileStorage;
};

} // namespace Application

#endif // APPLICATION_LIBRARY_CATALOG_SERVICE_HPP

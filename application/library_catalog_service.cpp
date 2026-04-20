#include "application/library_catalog_service.hpp"

namespace Application {

LibraryCatalogService::LibraryCatalogService(
    WritingsRepository &repository, WritingFileStorage &fileStorage)
    : m_repository(repository)
    , m_fileStorage(fileStorage)
{
}

QList<Domain::Writing> LibraryCatalogService::listWritings() const
{
    return m_repository.findAll();
}

OperationResult LibraryCatalogService::addWriting(const Domain::Writing &writing)
{
    const Domain::Writing normalized = writing.normalized();
    const QString error = normalized.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    return m_repository.add(normalized);
}

OperationResult LibraryCatalogService::updateWriting(
    const QString &currentTitle, const Domain::Writing &writing)
{
    const Domain::Writing normalized = writing.normalized();
    const QString error = normalized.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    return m_repository.update(currentTitle, normalized);
}

OperationResult LibraryCatalogService::removeWriting(const QString &title)
{
    return m_repository.remove(title);
}

OperationResult LibraryCatalogService::saveToFile(const QString &filePath) const
{
    return m_fileStorage.save(filePath, m_repository.findAll());
}

OperationResult LibraryCatalogService::loadFromFile(const QString &filePath)
{
    const LoadWritingsResult loadResult = m_fileStorage.load(filePath);

    if (!loadResult.ok) {
        return OperationResult::failure(loadResult.message);
    }

    const OperationResult replaceResult = m_repository.replaceAll(loadResult.writings);

    if (!replaceResult.ok) {
        return replaceResult;
    }

    return OperationResult::success(loadResult.message);
}

} // namespace Application

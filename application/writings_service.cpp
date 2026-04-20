#include "application/writings_service.hpp"

#include <QUuid>

namespace Application {

WritingsService::WritingsService(
    WritingsRepository &writingsRepository,
    AuthorsRepository &authorsRepository,
    WritingsFileStorage &fileStorage)
    : m_writingsRepository(writingsRepository)
    , m_authorsRepository(authorsRepository)
    , m_fileStorage(fileStorage)
{
}

QList<Domain::Writing> WritingsService::listWritings() const
{
    return m_writingsRepository.findAll();
}

OperationResult WritingsService::addWriting(const Domain::Writing &writing)
{
    Domain::Writing normalized = writing.normalized();
    const QString error = normalized.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    if (!m_authorsRepository.existsById(normalized.authorId())) {
        return OperationResult::failure(
            QStringLiteral("Нельзя сохранить произведение без существующего автора."));
    }

    if (normalized.id().isEmpty()) {
        normalized = normalized.withId(QUuid::createUuid().toString(QUuid::WithoutBraces));
    }

    return m_writingsRepository.add(normalized);
}

OperationResult WritingsService::updateWriting(
    const QString &writingId, const Domain::Writing &writing)
{
    const Domain::Writing normalized = writing.normalized().withId(writingId);
    const QString error = normalized.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    if (!m_authorsRepository.existsById(normalized.authorId())) {
        return OperationResult::failure(
            QStringLiteral("Нельзя сохранить произведение без существующего автора."));
    }

    return m_writingsRepository.update(writingId, normalized);
}

OperationResult WritingsService::removeWriting(const QString &writingId)
{
    return m_writingsRepository.remove(writingId);
}

OperationResult WritingsService::saveToFile(const QString &filePath) const
{
    return m_fileStorage.save(filePath, m_writingsRepository.findAll());
}

OperationResult WritingsService::loadFromFile(const QString &filePath)
{
    const LoadWritingsResult loadResult = m_fileStorage.load(filePath);

    if (!loadResult.ok) {
        return OperationResult::failure(loadResult.message);
    }

    for (const Domain::Writing &writing : loadResult.writings) {
        if (!m_authorsRepository.existsById(writing.authorId())) {
            return OperationResult::failure(
                QStringLiteral(
                    "Нельзя загрузить произведения: в файле есть ссылки на отсутствующих авторов."));
        }
    }

    const OperationResult replaceResult = m_writingsRepository.replaceAll(loadResult.writings);
    if (!replaceResult.ok) {
        return replaceResult;
    }

    return OperationResult::success(loadResult.message);
}

} // namespace Application

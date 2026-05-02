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

OperationResult WritingsService::addWriting(Domain::Writing writing)
{
    const QString error = writing.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    if (!m_authorsRepository.existsById(writing.authorId)) {
        return OperationResult::failure(
            QStringLiteral("нельзя сохранить произведение без существующего автора"));
    }

    if (writing.id.isNull()) {
        writing.id = QUuid::createUuid();
    }

    return m_writingsRepository.add(writing);
}

OperationResult WritingsService::updateWriting(const QUuid &writingId, Domain::Writing writing)
{
    writing.id = writingId;
    const QString error = writing.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    if (!m_authorsRepository.existsById(writing.authorId)) {
        return OperationResult::failure(
            QStringLiteral("нельзя сохранить произведение без существующего автора"));
    }

    return m_writingsRepository.update(writingId, writing);
}

OperationResult WritingsService::removeWriting(const QUuid &writingId)
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
        if (!m_authorsRepository.existsById(writing.authorId)) {
            return OperationResult::failure(QStringLiteral(
                "нельзя загрузить произведения: в файле есть ссылки на отсутствующих авторов"));
        }
    }

    const OperationResult replaceResult = m_writingsRepository.replaceAll(loadResult.writings);
    if (!replaceResult.ok) {
        return replaceResult;
    }

    return OperationResult::success(loadResult.message);
}

} // namespace Application

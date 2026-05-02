#include "application/writings_service.hpp"

#include <QUuid>

namespace Application {

WritingsService::WritingsService(
    WritingsRepository &writingsRepository,
    AuthorsRepository &authorsRepository,
    WritingsFileStorage &fileStorage)
    : _writingsRepository(writingsRepository)
    , _authorsRepository(authorsRepository)
    , _fileStorage(fileStorage)
{
}

QList<Domain::Writing> WritingsService::listWritings() const
{
    return _writingsRepository.findAll();
}

OperationResult WritingsService::addWriting(Domain::Writing writing)
{
    const QString error = writing.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    if (!_authorsRepository.existsById(writing.m_authorId)) {
        return OperationResult::failure(
            QStringLiteral("нельзя сохранить произведение без существующего автора"));
    }

    if (writing.m_id.isNull()) {
        writing.m_id = QUuid::createUuid();
    }

    return _writingsRepository.add(writing);
}

OperationResult WritingsService::updateWriting(const QUuid &writingId, Domain::Writing writing)
{
    writing.m_id = writingId;
    const QString error = writing.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    if (!_authorsRepository.existsById(writing.m_authorId)) {
        return OperationResult::failure(
            QStringLiteral("нельзя сохранить произведение без существующего автора"));
    }

    return _writingsRepository.update(writingId, writing);
}

OperationResult WritingsService::removeWriting(const QUuid &writingId)
{
    return _writingsRepository.remove(writingId);
}

OperationResult WritingsService::saveToFile(const QString &filePath) const
{
    return _fileStorage.save(filePath, _writingsRepository.findAll());
}

OperationResult WritingsService::loadFromFile(const QString &filePath)
{
    const LoadWritingsResult loadResult = _fileStorage.load(filePath);

    if (!loadResult.m_ok) {
        return OperationResult::failure(loadResult.m_message);
    }

    for (const Domain::Writing &writing : loadResult.m_writings) {
        if (!_authorsRepository.existsById(writing.m_authorId)) {
            return OperationResult::failure(QStringLiteral(
                "нельзя загрузить произведения: в файле есть ссылки на отсутствующих авторов"));
        }
    }

    const OperationResult replaceResult = _writingsRepository.replaceAll(loadResult.m_writings);
    if (!replaceResult.m_ok) {
        return replaceResult;
    }

    return OperationResult::success(loadResult.m_message);
}

} // namespace Application

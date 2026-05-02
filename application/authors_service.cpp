#include "application/authors_service.hpp"

#include <QSet>
#include <QUuid>

namespace Application {

AuthorsService::AuthorsService(
    AuthorsRepository &authorsRepository,
    WritingsRepository &writingsRepository,
    AuthorsFileStorage &fileStorage)
    : _authorsRepository(authorsRepository)
    , _writingsRepository(writingsRepository)
    , _fileStorage(fileStorage)
{
}

QList<Domain::Author> AuthorsService::listAuthors() const
{
    return _authorsRepository.findAll();
}

OperationResult AuthorsService::addAuthor(Domain::Author author)
{
    const QString error = author.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    if (author.m_id.isNull()) {
        author.m_id = QUuid::createUuid();
    }

    return _authorsRepository.add(author);
}

OperationResult AuthorsService::updateAuthor(const QUuid &authorId, Domain::Author author)
{
    author.m_id = authorId;
    const QString error = author.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    return _authorsRepository.update(authorId, author);
}

OperationResult AuthorsService::removeAuthor(const QUuid &authorId)
{
    if (_writingsRepository.hasForAuthor(authorId)) {
        return OperationResult::failure(
            QStringLiteral("нельзя удалить автора, пока у него есть произведения"));
    }

    return _authorsRepository.remove(authorId);
}

OperationResult AuthorsService::saveToFile(const QString &filePath) const
{
    return _fileStorage.save(filePath, _authorsRepository.findAll());
}

OperationResult AuthorsService::loadFromFile(const QString &filePath)
{
    const LoadAuthorsResult loadResult = _fileStorage.load(filePath);

    if (!loadResult.m_ok) {
        return OperationResult::failure(loadResult.m_message);
    }

    QSet<QUuid> loadedAuthorIds;
    for (const Domain::Author &author : loadResult.m_authors) {
        loadedAuthorIds.insert(author.m_id);
    }

    for (const Domain::Writing &writing : _writingsRepository.findAll()) {
        if (!loadedAuthorIds.contains(writing.m_authorId)) {
            return OperationResult::failure(
                QStringLiteral("нельзя загрузить авторов: текущие произведения ссылаются на "
                               "отсутствующих авторов"));
        }
    }

    const OperationResult replaceResult = _authorsRepository.replaceAll(loadResult.m_authors);
    if (!replaceResult.m_ok) {
        return replaceResult;
    }

    return OperationResult::success(loadResult.m_message);
}

} // namespace Application

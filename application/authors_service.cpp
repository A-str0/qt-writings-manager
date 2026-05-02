#include "application/authors_service.hpp"

#include <QSet>
#include <QUuid>

namespace Application {

AuthorsService::AuthorsService(
    AuthorsRepository &authorsRepository,
    WritingsRepository &writingsRepository,
    AuthorsFileStorage &fileStorage)
    : m_authorsRepository(authorsRepository)
    , m_writingsRepository(writingsRepository)
    , m_fileStorage(fileStorage)
{
}

QList<Domain::Author> AuthorsService::listAuthors() const
{
    return m_authorsRepository.findAll();
}

OperationResult AuthorsService::addAuthor(Domain::Author author)
{
    const QString error = author.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    if (author.id.isNull()) {
        author.id = QUuid::createUuid();
    }

    return m_authorsRepository.add(author);
}

OperationResult AuthorsService::updateAuthor(const QUuid &authorId, Domain::Author author)
{
    author.id = authorId;
    const QString error = author.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    return m_authorsRepository.update(authorId, author);
}

OperationResult AuthorsService::removeAuthor(const QUuid &authorId)
{
    if (m_writingsRepository.hasForAuthor(authorId)) {
        return OperationResult::failure(
            QStringLiteral("нельзя удалить автора, пока у него есть произведения"));
    }

    return m_authorsRepository.remove(authorId);
}

OperationResult AuthorsService::saveToFile(const QString &filePath) const
{
    return m_fileStorage.save(filePath, m_authorsRepository.findAll());
}

OperationResult AuthorsService::loadFromFile(const QString &filePath)
{
    const LoadAuthorsResult loadResult = m_fileStorage.load(filePath);

    if (!loadResult.ok) {
        return OperationResult::failure(loadResult.message);
    }

    QSet<QUuid> loadedAuthorIds;
    for (const Domain::Author &author : loadResult.authors) {
        loadedAuthorIds.insert(author.id);
    }

    for (const Domain::Writing &writing : m_writingsRepository.findAll()) {
        if (!loadedAuthorIds.contains(writing.authorId)) {
            return OperationResult::failure(
                QStringLiteral("нельзя загрузить авторов: текущие произведения ссылаются на "
                               "отсутствующих авторов"));
        }
    }

    const OperationResult replaceResult = m_authorsRepository.replaceAll(loadResult.authors);
    if (!replaceResult.ok) {
        return replaceResult;
    }

    return OperationResult::success(loadResult.message);
}

} // namespace Application

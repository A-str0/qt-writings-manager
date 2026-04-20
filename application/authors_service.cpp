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

OperationResult AuthorsService::addAuthor(const Domain::Author &author)
{
    Domain::Author normalized = author.normalized();
    const QString error = normalized.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    if (normalized.id().isEmpty()) {
        normalized = normalized.withId(QUuid::createUuid().toString(QUuid::WithoutBraces));
    }

    return m_authorsRepository.add(normalized);
}

OperationResult AuthorsService::updateAuthor(const QString &authorId, const Domain::Author &author)
{
    const Domain::Author normalized = author.normalized().withId(authorId);
    const QString error = normalized.validationError();

    if (!error.isEmpty()) {
        return OperationResult::failure(error);
    }

    return m_authorsRepository.update(authorId, normalized);
}

OperationResult AuthorsService::removeAuthor(const QString &authorId)
{
    if (m_writingsRepository.hasForAuthor(authorId)) {
        return OperationResult::failure(
            QStringLiteral("Нельзя удалить автора, пока у него есть произведения."));
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

    QSet<QString> loadedAuthorIds;
    for (const Domain::Author &author : loadResult.authors) {
        loadedAuthorIds.insert(author.id());
    }

    for (const Domain::Writing &writing : m_writingsRepository.findAll()) {
        if (!loadedAuthorIds.contains(writing.authorId())) {
            return OperationResult::failure(
                QStringLiteral(
                    "Нельзя загрузить авторов: текущие произведения ссылаются на отсутствующих авторов."));
        }
    }

    const OperationResult replaceResult = m_authorsRepository.replaceAll(loadResult.authors);
    if (!replaceResult.ok) {
        return replaceResult;
    }

    return OperationResult::success(loadResult.message);
}

} // namespace Application

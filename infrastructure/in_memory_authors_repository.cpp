#include "infrastructure/in_memory_authors_repository.hpp"

namespace Infrastructure {

QList<Domain::Author> InMemoryAuthorsRepository::findAll() const
{
    return m_authors.values();
}

bool InMemoryAuthorsRepository::existsById(const QUuid &authorId) const
{
    return m_authors.contains(authorId);
}

Application::OperationResult InMemoryAuthorsRepository::add(const Domain::Author &author)
{
    if (author.id.isNull()) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось сохранить автора без идентификатора."));
    }

    if (m_authors.contains(author.id)) {
        return Application::OperationResult::failure(
            QStringLiteral("Автор с таким идентификатором уже существует."));
    }

    m_authors.insert(author.id, author);
    return Application::OperationResult::success(QStringLiteral("Автор добавлен."));
}

Application::OperationResult InMemoryAuthorsRepository::update(
    const QUuid &authorId, const Domain::Author &author)
{
    if (!m_authors.contains(authorId)) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось найти автора для редактирования."));
    }

    m_authors.insert(authorId, author);
    return Application::OperationResult::success(QStringLiteral("Автор обновлён."));
}

Application::OperationResult InMemoryAuthorsRepository::remove(const QUuid &authorId)
{
    if (!m_authors.contains(authorId)) {
        return Application::OperationResult::failure(
            QStringLiteral("Выбранный автор уже отсутствует в каталоге."));
    }

    m_authors.remove(authorId);
    return Application::OperationResult::success(QStringLiteral("Автор удалён."));
}

Application::OperationResult InMemoryAuthorsRepository::replaceAll(
    const QList<Domain::Author> &authors)
{
    QMap<QUuid, Domain::Author> nextAuthors;

    for (const Domain::Author &author : authors) {
        const QString error = author.validationError();
        if (!error.isEmpty()) {
            return Application::OperationResult::failure(error);
        }

        if (author.id.isNull()) {
            return Application::OperationResult::failure(
                QStringLiteral("В CSV найден автор без идентификатора."));
        }

        if (nextAuthors.contains(author.id)) {
            return Application::OperationResult::failure(
                QStringLiteral("В CSV найдено несколько авторов с одинаковым идентификатором."));
        }

        nextAuthors.insert(author.id, author);
    }

    m_authors = nextAuthors;
    return Application::OperationResult::success();
}

} // namespace Infrastructure

#include "infrastructure/in_memory_authors_repository.hpp"

#include <utility>

namespace Infrastructure {

QString InMemoryAuthorsRepository::keyForId(const QString &id)
{
    return id.trimmed();
}

QString InMemoryAuthorsRepository::keyForName(const QString &name)
{
    return name.trimmed().toCaseFolded();
}

bool InMemoryAuthorsRepository::authorNameExists(
    const QString &name, const QString &exceptAuthorId) const
{
    const QString expectedNameKey = keyForName(name);
    const QString exceptKey = keyForId(exceptAuthorId);

    for (auto it = m_authors.cbegin(); it != m_authors.cend(); ++it) {
        if (it.key() == exceptKey) {
            continue;
        }

        if (keyForName(it.value().name()) == expectedNameKey) {
            return true;
        }
    }

    return false;
}

QList<Domain::Author> InMemoryAuthorsRepository::findAll() const
{
    return m_authors.values();
}

bool InMemoryAuthorsRepository::existsById(const QString &authorId) const
{
    return m_authors.contains(keyForId(authorId));
}

Application::OperationResult InMemoryAuthorsRepository::add(const Domain::Author &author)
{
    const QString authorKey = keyForId(author.id());

    if (authorKey.isEmpty()) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось сохранить автора без идентификатора."));
    }

    if (m_authors.contains(authorKey)) {
        return Application::OperationResult::failure(
            QStringLiteral("Автор с таким идентификатором уже существует."));
    }

    if (authorNameExists(author.name())) {
        return Application::OperationResult::failure(
            QStringLiteral("Автор с таким именем уже есть в каталоге."));
    }

    m_authors.insert(authorKey, author);
    return Application::OperationResult::success(QStringLiteral("Автор добавлен."));
}

Application::OperationResult InMemoryAuthorsRepository::update(
    const QString &authorId, const Domain::Author &author)
{
    const QString authorKey = keyForId(authorId);

    if (!m_authors.contains(authorKey)) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось найти автора для редактирования."));
    }

    if (authorNameExists(author.name(), authorKey)) {
        return Application::OperationResult::failure(
            QStringLiteral("Автор с таким именем уже есть в каталоге."));
    }

    m_authors.insert(authorKey, author);
    return Application::OperationResult::success(QStringLiteral("Автор обновлён."));
}

Application::OperationResult InMemoryAuthorsRepository::remove(const QString &authorId)
{
    const QString authorKey = keyForId(authorId);

    if (!m_authors.contains(authorKey)) {
        return Application::OperationResult::failure(
            QStringLiteral("Выбранный автор уже отсутствует в каталоге."));
    }

    m_authors.remove(authorKey);
    return Application::OperationResult::success(QStringLiteral("Автор удалён."));
}

Application::OperationResult InMemoryAuthorsRepository::replaceAll(
    const QList<Domain::Author> &authors)
{
    QMap<QString, Domain::Author> nextAuthors;

    for (const Domain::Author &author : authors) {
        const QString error = author.validationError();
        if (!error.isEmpty()) {
            return Application::OperationResult::failure(error);
        }

        const QString authorKey = keyForId(author.id());
        if (authorKey.isEmpty()) {
            return Application::OperationResult::failure(
                QStringLiteral("В CSV найден автор без идентификатора."));
        }

        if (nextAuthors.contains(authorKey)) {
            return Application::OperationResult::failure(
                QStringLiteral("В CSV найдено несколько авторов с одинаковым идентификатором."));
        }

        for (const Domain::Author &existingAuthor : nextAuthors) {
            if (keyForName(existingAuthor.name()) == keyForName(author.name())) {
                return Application::OperationResult::failure(
                    QStringLiteral("В CSV найдено несколько авторов с одинаковым именем."));
            }
        }

        nextAuthors.insert(authorKey, author);
    }

    m_authors = std::move(nextAuthors);
    return Application::OperationResult::success();
}

} // namespace Infrastructure

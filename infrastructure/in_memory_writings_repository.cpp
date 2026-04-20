#include "infrastructure/in_memory_writings_repository.hpp"

#include <utility>

namespace Infrastructure {

QString InMemoryWritingsRepository::keyForId(const QString &id)
{
    return id.trimmed();
}

QList<Domain::Writing> InMemoryWritingsRepository::findAll() const
{
    return m_writings.values();
}

bool InMemoryWritingsRepository::hasForAuthor(const QString &authorId) const
{
    const QString authorKey = keyForId(authorId);

    for (const Domain::Writing &writing : m_writings) {
        if (keyForId(writing.authorId()) == authorKey) {
            return true;
        }
    }

    return false;
}

Application::OperationResult InMemoryWritingsRepository::add(const Domain::Writing &writing)
{
    const QString writingKey = keyForId(writing.id());

    if (writingKey.isEmpty()) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось сохранить произведение без идентификатора."));
    }

    if (m_writings.contains(writingKey)) {
        return Application::OperationResult::failure(
            QStringLiteral("Произведение с таким идентификатором уже существует."));
    }

    m_writings.insert(writingKey, writing);
    return Application::OperationResult::success(QStringLiteral("Произведение добавлено."));
}

Application::OperationResult InMemoryWritingsRepository::update(
    const QString &writingId, const Domain::Writing &writing)
{
    const QString writingKey = keyForId(writingId);

    if (!m_writings.contains(writingKey)) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось найти произведение для редактирования."));
    }

    m_writings.insert(writingKey, writing);
    return Application::OperationResult::success(QStringLiteral("Произведение обновлено."));
}

Application::OperationResult InMemoryWritingsRepository::remove(const QString &writingId)
{
    const QString writingKey = keyForId(writingId);

    if (!m_writings.contains(writingKey)) {
        return Application::OperationResult::failure(
            QStringLiteral("Выбранное произведение уже отсутствует в каталоге."));
    }

    m_writings.remove(writingKey);
    return Application::OperationResult::success(QStringLiteral("Произведение удалено."));
}

Application::OperationResult InMemoryWritingsRepository::replaceAll(
    const QList<Domain::Writing> &writings)
{
    QMap<QString, Domain::Writing> nextWritings;

    for (const Domain::Writing &writing : writings) {
        const QString error = writing.validationError();
        if (!error.isEmpty()) {
            return Application::OperationResult::failure(error);
        }

        const QString writingKey = keyForId(writing.id());

        if (writingKey.isEmpty()) {
            return Application::OperationResult::failure(
                QStringLiteral("В CSV найдено произведение без идентификатора."));
        }

        if (nextWritings.contains(writingKey)) {
            return Application::OperationResult::failure(
                QStringLiteral(
                    "В CSV найдено несколько произведений с одинаковым идентификатором."));
        }

        nextWritings.insert(writingKey, writing);
    }

    m_writings = std::move(nextWritings);
    return Application::OperationResult::success();
}

} // namespace Infrastructure

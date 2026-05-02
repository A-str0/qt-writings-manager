#include "infrastructure/in_memory_writings_repository.hpp"

namespace Infrastructure {

QList<Domain::Writing> InMemoryWritingsRepository::findAll() const
{
    return m_writings.values();
}

bool InMemoryWritingsRepository::hasForAuthor(const QUuid &authorId) const
{
    for (const Domain::Writing &writing : m_writings) {
        if (writing.authorId == authorId) {
            return true;
        }
    }

    return false;
}

Application::OperationResult InMemoryWritingsRepository::add(const Domain::Writing &writing)
{
    if (writing.id.isNull()) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось сохранить произведение без идентификатора."));
    }

    if (m_writings.contains(writing.id)) {
        return Application::OperationResult::failure(
            QStringLiteral("Произведение с таким идентификатором уже существует."));
    }

    m_writings.insert(writing.id, writing);
    return Application::OperationResult::success(QStringLiteral("Произведение добавлено."));
}

Application::OperationResult InMemoryWritingsRepository::update(
    const QUuid &writingId, const Domain::Writing &writing)
{
    if (!m_writings.contains(writingId)) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось найти произведение для редактирования."));
    }

    m_writings.insert(writingId, writing);
    return Application::OperationResult::success(QStringLiteral("Произведение обновлено."));
}

Application::OperationResult InMemoryWritingsRepository::remove(const QUuid &writingId)
{
    if (!m_writings.contains(writingId)) {
        return Application::OperationResult::failure(
            QStringLiteral("Выбранное произведение уже отсутствует в каталоге."));
    }

    m_writings.remove(writingId);
    return Application::OperationResult::success(QStringLiteral("Произведение удалено."));
}

Application::OperationResult InMemoryWritingsRepository::replaceAll(
    const QList<Domain::Writing> &writings)
{
    QMap<QUuid, Domain::Writing> nextWritings;

    for (const Domain::Writing &writing : writings) {
        const QString error = writing.validationError();
        if (!error.isEmpty()) {
            return Application::OperationResult::failure(error);
        }

        if (writing.id.isNull()) {
            return Application::OperationResult::failure(
                QStringLiteral("В CSV найдено произведение без идентификатора."));
        }

        if (nextWritings.contains(writing.id)) {
            return Application::OperationResult::failure(
                QStringLiteral(
                    "В CSV найдено несколько произведений с одинаковым идентификатором."));
        }

        nextWritings.insert(writing.id, writing);
    }

    m_writings = nextWritings;
    return Application::OperationResult::success();
}

} // namespace Infrastructure

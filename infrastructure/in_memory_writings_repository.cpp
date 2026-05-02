#include "infrastructure/in_memory_writings_repository.hpp"

namespace Infrastructure {

QList<Domain::Writing> InMemoryWritingsRepository::findAll() const
{
    return _writings.values();
}

bool InMemoryWritingsRepository::hasForAuthor(const QUuid &authorId) const
{
    for (const Domain::Writing &writing : _writings) {
        if (writing.m_authorId == authorId) {
            return true;
        }
    }

    return false;
}

Application::OperationResult InMemoryWritingsRepository::add(const Domain::Writing &writing)
{
    if (writing.m_id.isNull()) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось сохранить произведение без идентификатора."));
    }

    if (_writings.contains(writing.m_id)) {
        return Application::OperationResult::failure(
            QStringLiteral("Произведение с таким идентификатором уже существует."));
    }

    _writings.insert(writing.m_id, writing);
    return Application::OperationResult::success(QStringLiteral("Произведение добавлено."));
}

Application::OperationResult InMemoryWritingsRepository::update(
    const QUuid &writingId, const Domain::Writing &writing)
{
    if (!_writings.contains(writingId)) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось найти произведение для редактирования."));
    }

    _writings.insert(writingId, writing);
    return Application::OperationResult::success(QStringLiteral("Произведение обновлено."));
}

Application::OperationResult InMemoryWritingsRepository::remove(const QUuid &writingId)
{
    if (!_writings.contains(writingId)) {
        return Application::OperationResult::failure(
            QStringLiteral("Выбранное произведение уже отсутствует в каталоге."));
    }

    _writings.remove(writingId);
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

        if (writing.m_id.isNull()) {
            return Application::OperationResult::failure(
                QStringLiteral("В CSV найдено произведение без идентификатора."));
        }

        if (nextWritings.contains(writing.m_id)) {
            return Application::OperationResult::failure(
                QStringLiteral(
                    "В CSV найдено несколько произведений с одинаковым идентификатором."));
        }

        nextWritings.insert(writing.m_id, writing);
    }

    _writings = nextWritings;
    return Application::OperationResult::success();
}

} // namespace Infrastructure

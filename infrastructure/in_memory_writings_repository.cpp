#include "infrastructure/in_memory_writings_repository.hpp"

namespace Infrastructure {

QString InMemoryWritingsRepository::keyForTitle(const QString &title)
{
    return title.trimmed().toCaseFolded();
}

QList<Domain::Writing> InMemoryWritingsRepository::findAll() const
{
    return m_writings.values();
}

Application::OperationResult InMemoryWritingsRepository::add(const Domain::Writing &writing)
{
    const QString key = keyForTitle(writing.title());

    if (m_writings.contains(key)) {
        return Application::OperationResult::failure(
            QStringLiteral("Произведение с таким названием уже есть в каталоге."));
    }

    m_writings.insert(key, writing);
    return Application::OperationResult::success(QStringLiteral("Запись добавлена в каталог."));
}

Application::OperationResult InMemoryWritingsRepository::update(
    const QString &currentTitle, const Domain::Writing &writing)
{
    const QString currentKey = keyForTitle(currentTitle);

    if (!m_writings.contains(currentKey)) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось найти запись для редактирования."));
    }

    const QString newKey = keyForTitle(writing.title());

    if (newKey != currentKey && m_writings.contains(newKey)) {
        return Application::OperationResult::failure(
            QStringLiteral("Нельзя переименовать запись: такое произведение уже существует."));
    }

    if (newKey != currentKey) {
        m_writings.remove(currentKey);
    }

    m_writings.insert(newKey, writing);
    return Application::OperationResult::success(QStringLiteral("Запись обновлена."));
}

Application::OperationResult InMemoryWritingsRepository::remove(const QString &title)
{
    const QString key = keyForTitle(title);

    if (!m_writings.contains(key)) {
        return Application::OperationResult::failure(
            QStringLiteral("Выбранная запись уже отсутствует в каталоге."));
    }

    m_writings.remove(key);
    return Application::OperationResult::success(QStringLiteral("Запись удалена."));
}

Application::OperationResult InMemoryWritingsRepository::replaceAll(
    const QList<Domain::Writing> &writings)
{
    QMap<QString, Domain::Writing> nextState;

    for (const Domain::Writing &writing : writings) {
        const QString error = writing.validationError();
        if (!error.isEmpty()) {
            return Application::OperationResult::failure(error);
        }

        const QString key = keyForTitle(writing.title());

        if (nextState.contains(key)) {
            return Application::OperationResult::failure(
                QStringLiteral("В CSV найдено несколько произведений с одинаковым названием."));
        }

        nextState.insert(key, writing);
    }

    m_writings = std::move(nextState);
    return Application::OperationResult::success();
}

} // namespace Infrastructure

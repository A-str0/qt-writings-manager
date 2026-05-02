#include "infrastructure/csv_writings_file_storage.hpp"
#include "infrastructure/csv_file_storage_utils.hpp"

#include <QFile>
#include <QSaveFile>
#include <QStringConverter>
#include <QTextStream>

namespace {

const QString kWritingsHeader = QStringLiteral("id,title,authorId,description");
namespace Csv = Infrastructure::CsvFileStorageDetail;

constexpr qsizetype kWritingColumnCount = 4;
constexpr int kWritingIdColumn = 0;
constexpr int kWritingTitleColumn = 1;
constexpr int kWritingAuthorIdColumn = 2;
constexpr int kWritingDescriptionColumn = 3;

} // namespace

namespace Infrastructure {

Application::OperationResult CsvWritingsFileStorage::save(
    const QString &filePath, const QList<Domain::Writing> &writings) const
{
    if (filePath.trimmed().isEmpty()) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось определить путь для сохранения произведений."));
    }

    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось открыть файл произведений для записи: %1")
                .arg(file.errorString()));
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << kWritingsHeader << '\n';

    for (const Domain::Writing &writing : writings) {
        stream << Csv::joinLine(QStringList{
                      Csv::encodeField(Csv::serializeUuid(writing.id)),
                      Csv::encodeField(writing.title),
                      Csv::encodeField(Csv::serializeUuid(writing.authorId)),
                      Csv::encodeField(writing.description)})
               << '\n';
    }

    if (!file.commit()) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось сохранить CSV-файл произведений."));
    }

    return Application::OperationResult::success(
        QStringLiteral("Произведения сохранены в CSV (%1 записей).").arg(writings.size()));
}

Application::LoadWritingsResult CsvWritingsFileStorage::load(const QString &filePath) const
{
    if (filePath.trimmed().isEmpty()) {
        return Application::LoadWritingsResult::failure(
            QStringLiteral("Не удалось определить путь к файлу произведений."));
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return Application::LoadWritingsResult::failure(
            QStringLiteral("Не удалось открыть CSV-файл произведений: %1")
                .arg(file.errorString()));
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    if (stream.readLine() != kWritingsHeader) {
        return Application::LoadWritingsResult::failure(
            QStringLiteral("Неверный заголовок CSV-файла произведений."));
    }

    QList<Domain::Writing> writings;
    int lineNumber = 1;

    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        ++lineNumber;

        if (line.isEmpty()) {
            continue;
        }

        const QStringList columns = Csv::splitLine(line);
        if (columns.size() != kWritingColumnCount) {
            return Application::LoadWritingsResult::failure(
                QStringLiteral("Не удалось разобрать строку %1 в файле произведений.")
                    .arg(lineNumber));
        }

        const QUuid writingId = QUuid::fromString(Csv::decodeField(columns.at(kWritingIdColumn)));
        if (writingId.isNull()) {
            return Application::LoadWritingsResult::failure(
                QStringLiteral("Некорректный идентификатор произведения в строке %1.")
                    .arg(lineNumber));
        }

        const QUuid authorId =
            QUuid::fromString(Csv::decodeField(columns.at(kWritingAuthorIdColumn)));
        if (authorId.isNull()) {
            return Application::LoadWritingsResult::failure(
                QStringLiteral("Некорректный идентификатор автора в строке %1.")
                    .arg(lineNumber));
        }

        const Domain::Writing writing(
            writingId,
            authorId,
            Csv::decodeField(columns.at(kWritingTitleColumn)),
            Csv::decodeField(columns.at(kWritingDescriptionColumn)));
        const QString error = writing.validationError();
        if (!error.isEmpty()) {
            return Application::LoadWritingsResult::failure(
                QStringLiteral("Ошибка в строке %1: %2").arg(lineNumber).arg(error));
        }

        writings.append(writing);
    }

    return Application::LoadWritingsResult::success(
        writings,
        QStringLiteral("Произведения загружены из CSV (%1 записей).").arg(writings.size()));
}

} // namespace Infrastructure

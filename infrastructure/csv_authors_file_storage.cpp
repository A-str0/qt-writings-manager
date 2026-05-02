#include "infrastructure/csv_authors_file_storage.hpp"
#include "infrastructure/csv_file_storage_utils.hpp"

#include <QFile>
#include <QSaveFile>
#include <QStringConverter>
#include <QTextStream>

namespace {

const QString kAuthorsHeader = QStringLiteral("id,name");
namespace Csv = Infrastructure::CsvFileStorageDetail;

constexpr qsizetype kAuthorColumnCount = 2;
constexpr int kAuthorIdColumn = 0;
constexpr int kAuthorNameColumn = 1;

} // namespace

namespace Infrastructure {

Application::OperationResult CsvAuthorsFileStorage::save(
    const QString &filePath, const QList<Domain::Author> &authors) const
{
    if (filePath.trimmed().isEmpty()) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось определить путь для сохранения авторов."));
    }

    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось открыть файл авторов для записи: %1")
                .arg(file.errorString()));
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << kAuthorsHeader << '\n';

    for (const Domain::Author &author : authors) {
        stream << Csv::joinLine(QStringList{
                      Csv::encodeField(Csv::serializeUuid(author.m_id)),
                      Csv::encodeField(author.m_name)})
               << '\n';
    }

    if (!file.commit()) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось сохранить CSV-файл авторов."));
    }

    return Application::OperationResult::success(
        QStringLiteral("Авторы сохранены в CSV (%1 записей).").arg(authors.size()));
}

Application::LoadAuthorsResult CsvAuthorsFileStorage::load(const QString &filePath) const
{
    if (filePath.trimmed().isEmpty()) {
        return Application::LoadAuthorsResult::failure(
            QStringLiteral("Не удалось определить путь к файлу авторов."));
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return Application::LoadAuthorsResult::failure(
            QStringLiteral("Не удалось открыть CSV-файл авторов: %1").arg(file.errorString()));
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    if (stream.readLine() != kAuthorsHeader) {
        return Application::LoadAuthorsResult::failure(
            QStringLiteral("Неверный заголовок CSV-файла авторов."));
    }

    QList<Domain::Author> authors;
    int lineNumber = 1;

    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        ++lineNumber;

        if (line.isEmpty()) {
            continue;
        }

        const QStringList columns = Csv::splitLine(line);
        if (columns.size() != kAuthorColumnCount) {
            return Application::LoadAuthorsResult::failure(
                QStringLiteral("Не удалось разобрать строку %1 в файле авторов.")
                    .arg(lineNumber));
        }

        const QUuid authorId = QUuid::fromString(Csv::decodeField(columns.at(kAuthorIdColumn)));
        if (authorId.isNull()) {
            return Application::LoadAuthorsResult::failure(
                QStringLiteral("Некорректный идентификатор автора в строке %1.")
                    .arg(lineNumber));
        }

        const Domain::Author author(authorId, Csv::decodeField(columns.at(kAuthorNameColumn)));
        const QString error = author.validationError();
        if (!error.isEmpty()) {
            return Application::LoadAuthorsResult::failure(
                QStringLiteral("Ошибка в строке %1: %2").arg(lineNumber).arg(error));
        }

        authors.append(author);
    }

    return Application::LoadAuthorsResult::success(
        authors, QStringLiteral("Авторы загружены из CSV (%1 записей).").arg(authors.size()));
}

} // namespace Infrastructure

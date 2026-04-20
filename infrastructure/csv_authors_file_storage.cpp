#include "infrastructure/csv_authors_file_storage.hpp"

#include <QFile>
#include <QSaveFile>
#include <QStringConverter>
#include <QStringList>
#include <QTextStream>

namespace {

const QString kAuthorsHeader = QStringLiteral("id,name,birthYear,country");

QString escapeCsvField(QString value)
{
    value.replace("\r\n", "\n");
    value.replace('\r', '\n');
    value.replace('\n', "\\n");
    value.replace('"', "\"\"");
    return '"' + value + '"';
}

QString unescapeCsvField(QString value)
{
    value.replace("\\n", "\n");
    return value;
}

QStringList parseCsvLine(const QString &line, bool &ok)
{
    QStringList values;
    QString current;
    bool inQuotes = false;

    for (int index = 0; index < line.size(); ++index) {
        const QChar character = line.at(index);

        if (inQuotes) {
            if (character == '"') {
                if (index + 1 < line.size() && line.at(index + 1) == '"') {
                    current += '"';
                    ++index;
                } else {
                    inQuotes = false;
                }
            } else {
                current += character;
            }
        } else if (character == '"') {
            inQuotes = true;
        } else if (character == ',') {
            values.append(current);
            current.clear();
        } else {
            current += character;
        }
    }

    ok = !inQuotes;
    if (ok) {
        values.append(current);
    }

    return values;
}

bool parseOptionalInt(const QString &text, int &value)
{
    value = 0;
    const QString trimmed = text.trimmed();

    if (trimmed.isEmpty()) {
        return true;
    }

    bool ok = false;
    value = trimmed.toInt(&ok);
    return ok;
}

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
        stream << escapeCsvField(author.id()) << ',' << escapeCsvField(author.name()) << ','
               << escapeCsvField(
                      author.birthYear() == 0 ? QString() : QString::number(author.birthYear()))
               << ',' << escapeCsvField(author.country()) << '\n';
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

    QList<Domain::Author> authors;
    int lineNumber = 0;

    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        ++lineNumber;

        if (lineNumber == 1) {
            if (line.trimmed() != kAuthorsHeader) {
                return Application::LoadAuthorsResult::failure(
                    QStringLiteral("Неверный заголовок CSV-файла авторов."));
            }
            continue;
        }

        if (line.trimmed().isEmpty()) {
            continue;
        }

        bool ok = false;
        const QStringList columns = parseCsvLine(line, ok);

        if (!ok || columns.size() != 4) {
            return Application::LoadAuthorsResult::failure(
                QStringLiteral("Не удалось разобрать строку %1 в файле авторов.")
                    .arg(lineNumber));
        }

        int birthYear = 0;
        if (!parseOptionalInt(columns.at(2), birthYear)) {
            return Application::LoadAuthorsResult::failure(
                QStringLiteral("Некорректный год рождения автора в строке %1.").arg(lineNumber));
        }

        const Domain::Author author(
            unescapeCsvField(columns.at(0)),
            unescapeCsvField(columns.at(1)),
            birthYear,
            unescapeCsvField(columns.at(3)));

        const QString error = author.validationError();
        if (!error.isEmpty()) {
            return Application::LoadAuthorsResult::failure(
                QStringLiteral("Ошибка в строке %1: %2").arg(lineNumber).arg(error));
        }

        authors.append(author.normalized());
    }

    return Application::LoadAuthorsResult::success(
        authors, QStringLiteral("Авторы загружены из CSV (%1 записей).").arg(authors.size()));
}

} // namespace Infrastructure

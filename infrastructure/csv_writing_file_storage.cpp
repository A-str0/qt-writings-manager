#include "infrastructure/csv_writing_file_storage.hpp"

#include <QFile>
#include <QSaveFile>
#include <QStringConverter>
#include <QStringList>
#include <QTextStream>

namespace {

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

} // namespace

namespace Infrastructure {

Application::OperationResult CsvWritingFileStorage::save(
    const QString &filePath, const QList<Domain::Writing> &writings) const
{
    if (filePath.trimmed().isEmpty()) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось определить путь для сохранения."));
    }

    QSaveFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось открыть файл для записи: %1").arg(file.errorString()));
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << "title,author,genre,year,description\n";

    for (const Domain::Writing &writing : writings) {
        stream << escapeCsvField(writing.title()) << ',' << escapeCsvField(writing.author()) << ','
               << escapeCsvField(writing.genre()) << ','
               << escapeCsvField(writing.publicationYear() == 0
                                     ? QString()
                                     : QString::number(writing.publicationYear()))
               << ',' << escapeCsvField(writing.description()) << '\n';
    }

    if (!file.commit()) {
        return Application::OperationResult::failure(
            QStringLiteral("Не удалось сохранить CSV-файл."));
    }

    return Application::OperationResult::success(
        QStringLiteral("Каталог сохранён в CSV (%1 записей).").arg(writings.size()));
}

Application::LoadWritingsResult CsvWritingFileStorage::load(const QString &filePath) const
{
    if (filePath.trimmed().isEmpty()) {
        return Application::LoadWritingsResult::failure(
            QStringLiteral("Не удалось определить путь к файлу."));
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return Application::LoadWritingsResult::failure(
            QStringLiteral("Не удалось открыть CSV-файл: %1").arg(file.errorString()));
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);

    QList<Domain::Writing> writings;
    int lineNumber = 0;

    while (!stream.atEnd()) {
        QString line = stream.readLine();
        ++lineNumber;

        if (lineNumber == 1 && line.trimmed() == "title,author,genre,year,description") {
            continue;
        }

        if (line.trimmed().isEmpty()) {
            continue;
        }

        bool ok = false;
        const QStringList columns = parseCsvLine(line, ok);

        if (!ok || columns.size() != 5) {
            return Application::LoadWritingsResult::failure(
                QStringLiteral("Не удалось разобрать CSV-строку %1.").arg(lineNumber));
        }

        bool yearOk = true;
        int year = 0;
        const QString yearText = columns.at(3).trimmed();

        if (!yearText.isEmpty()) {
            year = yearText.toInt(&yearOk);
        }

        if (!yearOk) {
            return Application::LoadWritingsResult::failure(
                QStringLiteral("Некорректный год издания в строке %1.").arg(lineNumber));
        }

        const Domain::Writing writing(
            unescapeCsvField(columns.at(0)),
            unescapeCsvField(columns.at(1)),
            unescapeCsvField(columns.at(2)),
            year,
            unescapeCsvField(columns.at(4)));

        const QString error = writing.validationError();
        if (!error.isEmpty()) {
            return Application::LoadWritingsResult::failure(
                QStringLiteral("Ошибка в строке %1: %2").arg(lineNumber).arg(error));
        }

        writings.append(writing.normalized());
    }

    return Application::LoadWritingsResult::success(
        writings, QStringLiteral("Каталог загружен из CSV (%1 записей).").arg(writings.size()));
}

} // namespace Infrastructure

#include "infrastructure/csv_writings_file_storage.hpp"

#include <QFile>
#include <QSaveFile>
#include <QStringConverter>
#include <QStringList>
#include <QTextStream>

namespace {

const QString kWritingsHeader =
    QStringLiteral("id,title,authorId,genre,publicationYear,description");

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
        stream << escapeCsvField(writing.id()) << ',' << escapeCsvField(writing.title()) << ','
               << escapeCsvField(writing.authorId()) << ',' << escapeCsvField(writing.genre())
               << ','
               << escapeCsvField(writing.publicationYear() == 0
                                     ? QString()
                                     : QString::number(writing.publicationYear()))
               << ',' << escapeCsvField(writing.description()) << '\n';
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

    QList<Domain::Writing> writings;
    int lineNumber = 0;

    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        ++lineNumber;

        if (lineNumber == 1) {
            if (line.trimmed() != kWritingsHeader) {
                return Application::LoadWritingsResult::failure(
                    QStringLiteral("Неверный заголовок CSV-файла произведений."));
            }
            continue;
        }

        if (line.trimmed().isEmpty()) {
            continue;
        }

        bool ok = false;
        const QStringList columns = parseCsvLine(line, ok);

        if (!ok || columns.size() != 6) {
            return Application::LoadWritingsResult::failure(
                QStringLiteral("Не удалось разобрать строку %1 в файле произведений.")
                    .arg(lineNumber));
        }

        int publicationYear = 0;
        if (!parseOptionalInt(columns.at(4), publicationYear)) {
            return Application::LoadWritingsResult::failure(
                QStringLiteral("Некорректный год издания в строке %1.").arg(lineNumber));
        }

        const Domain::Writing writing(
            unescapeCsvField(columns.at(0)),
            unescapeCsvField(columns.at(1)),
            unescapeCsvField(columns.at(2)),
            unescapeCsvField(columns.at(3)),
            publicationYear,
            unescapeCsvField(columns.at(5)));

        const QString error = writing.validationError();
        if (!error.isEmpty()) {
            return Application::LoadWritingsResult::failure(
                QStringLiteral("Ошибка в строке %1: %2").arg(lineNumber).arg(error));
        }

        writings.append(writing.normalized());
    }

    return Application::LoadWritingsResult::success(
        writings,
        QStringLiteral("Произведения загружены из CSV (%1 записей).").arg(writings.size()));
}

} // namespace Infrastructure

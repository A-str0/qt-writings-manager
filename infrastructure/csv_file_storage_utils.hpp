#ifndef INFRASTRUCTURE_CSV_FILE_STORAGE_UTILS_HPP
#define INFRASTRUCTURE_CSV_FILE_STORAGE_UTILS_HPP

#include <QString>
#include <QStringList>
#include <QUuid>
#include <QUrl>

namespace Infrastructure::CsvFileStorageDetail {

inline QString encodeField(const QString &value)
{
    return QString::fromLatin1(QUrl::toPercentEncoding(value));
}

inline QString decodeField(const QString &value)
{
    return QUrl::fromPercentEncoding(value.toLatin1());
}

inline QString serializeUuid(const QUuid &id)
{
    return id.toString(QUuid::WithoutBraces);
}

inline QStringList splitLine(const QString &line)
{
    return line.split(u',', Qt::KeepEmptyParts);
}

inline QString joinLine(const QStringList &columns)
{
    return columns.join(u',');
}

} // namespace Infrastructure::CsvFileStorageDetail

#endif // INFRASTRUCTURE_CSV_FILE_STORAGE_UTILS_HPP

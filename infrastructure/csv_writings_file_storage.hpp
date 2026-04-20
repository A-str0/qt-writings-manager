#ifndef INFRASTRUCTURE_CSV_WRITINGS_FILE_STORAGE_HPP
#define INFRASTRUCTURE_CSV_WRITINGS_FILE_STORAGE_HPP

#include "application/writings_file_storage.hpp"

namespace Infrastructure {

class CsvWritingsFileStorage final : public Application::WritingsFileStorage
{
public:
    Application::OperationResult save(
        const QString &filePath, const QList<Domain::Writing> &writings) const override;
    Application::LoadWritingsResult load(const QString &filePath) const override;
};

} // namespace Infrastructure

#endif // INFRASTRUCTURE_CSV_WRITINGS_FILE_STORAGE_HPP

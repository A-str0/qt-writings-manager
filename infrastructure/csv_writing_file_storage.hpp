#ifndef INFRASTRUCTURE_CSV_WRITING_FILE_STORAGE_HPP
#define INFRASTRUCTURE_CSV_WRITING_FILE_STORAGE_HPP

#include "application/writing_file_storage.hpp"

namespace Infrastructure {

class CsvWritingFileStorage final : public Application::WritingFileStorage
{
public:
    Application::OperationResult save(
        const QString &filePath, const QList<Domain::Writing> &writings) const override;
    Application::LoadWritingsResult load(const QString &filePath) const override;
};

} // namespace Infrastructure

#endif // INFRASTRUCTURE_CSV_WRITING_FILE_STORAGE_HPP

#ifndef INFRASTRUCTURE_CSV_AUTHORS_FILE_STORAGE_HPP
#define INFRASTRUCTURE_CSV_AUTHORS_FILE_STORAGE_HPP

#include "application/authors_file_storage.hpp"

namespace Infrastructure {

class CsvAuthorsFileStorage final : public Application::AuthorsFileStorage
{
public:
    Application::OperationResult save(
        const QString &filePath, const QList<Domain::Author> &authors) const override;
    Application::LoadAuthorsResult load(const QString &filePath) const override;
};

} // namespace Infrastructure

#endif // INFRASTRUCTURE_CSV_AUTHORS_FILE_STORAGE_HPP

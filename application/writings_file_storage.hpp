#ifndef APPLICATION_WRITINGS_FILE_STORAGE_HPP
#define APPLICATION_WRITINGS_FILE_STORAGE_HPP

#include <QList>
#include <QString>

#include "application/operation_result.hpp"

namespace Application {

class WritingsFileStorage
{
public:
    virtual ~WritingsFileStorage() = default;

    virtual OperationResult save(
        const QString &filePath, const QList<Domain::Writing> &writings) const = 0;
    virtual LoadWritingsResult load(const QString &filePath) const = 0;
};

} // namespace Application

#endif // APPLICATION_WRITINGS_FILE_STORAGE_HPP

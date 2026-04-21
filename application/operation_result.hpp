#ifndef APPLICATION_OPERATION_RESULT_HPP
#define APPLICATION_OPERATION_RESULT_HPP

#include <QList>
#include <QString>

#include <utility>

#include "domain/author.hpp"
#include "domain/writing.hpp"

namespace Application {

struct OperationResult
{
    bool ok = true;
    QString message;

    static OperationResult success(QString message = {}) { return {true, std::move(message)}; }
    static OperationResult failure(QString message) { return {false, std::move(message)}; }
};

struct LoadAuthorsResult
{
    bool ok = false;
    QString message;
    QList<Domain::Author> authors;

    static LoadAuthorsResult success(QList<Domain::Author> authors, QString message = {})
    {
        return {true, std::move(message), std::move(authors)};
    }

    static LoadAuthorsResult failure(QString message) { return {false, std::move(message), {}}; }
};

struct LoadWritingsResult
{
    bool ok = false;
    QString message;
    QList<Domain::Writing> writings;

    static LoadWritingsResult success(QList<Domain::Writing> writings, QString message = {})
    {
        return {true, std::move(message), std::move(writings)};
    }

    static LoadWritingsResult failure(QString message) { return {false, std::move(message), {}}; }
};

} // namespace Application

#endif // APPLICATION_OPERATION_RESULT_HPP

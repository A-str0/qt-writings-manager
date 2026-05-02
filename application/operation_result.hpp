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
    bool m_ok = true;
    QString m_message;

    static OperationResult success(QString message = {}) { return {true, std::move(message)}; }
    static OperationResult failure(QString message) { return {false, std::move(message)}; }
};

struct LoadAuthorsResult
{
    bool m_ok = false;
    QString m_message;
    QList<Domain::Author> m_authors;

    static LoadAuthorsResult success(QList<Domain::Author> authors, QString message = {})
    {
        return {true, std::move(message), std::move(authors)};
    }

    static LoadAuthorsResult failure(QString message) { return {false, std::move(message), {}}; }
};

struct LoadWritingsResult
{
    bool m_ok = false;
    QString m_message;
    QList<Domain::Writing> m_writings;

    static LoadWritingsResult success(QList<Domain::Writing> writings, QString message = {})
    {
        return {true, std::move(message), std::move(writings)};
    }

    static LoadWritingsResult failure(QString message) { return {false, std::move(message), {}}; }
};

} // namespace Application

#endif // APPLICATION_OPERATION_RESULT_HPP

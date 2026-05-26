#ifndef PRESENTATION_AUTHORS_TABLE_MODEL_HPP
#define PRESENTATION_AUTHORS_TABLE_MODEL_HPP

#include <QAbstractTableModel>
#include <QList>

#include "domain/author.hpp"

namespace Presentation {

class AuthorsTableModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    using QAbstractTableModel::QAbstractTableModel;

    int rowCount(const QModelIndex &parent = {}) const override { return parent.isValid() ? 0 : _authors.size(); }

    int columnCount(const QModelIndex &parent = {}) const override { return parent.isValid() ? 0 : 2; }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() >= _authors.size())
            return {};

        const auto &a = _authors[index.row()];

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case 0: return a.m_name;
            case 1: return a.m_id.toString(QUuid::WithoutBraces);
            }
        }

        if (role == Qt::UserRole)
            return a.m_id;

        return {};
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
            return {};

        switch (section) {
        case 0: return QStringLiteral("Имя автора");
        case 1: return QStringLiteral("ID");
        }
        return {};
    }

    void setAuthors(const QList<Domain::Author> &authors)
    {
        beginResetModel();
        _authors = authors;
        endResetModel();
    }

    const Domain::Author &authorAt(int row) const { return _authors[row]; }

    int rowByAuthorId(const QUuid &id) const
    {
        for (int i = 0; i < _authors.size(); ++i)
            if (_authors[i].m_id == id) return i;
        return -1;
    }

private:
    QList<Domain::Author> _authors;
};

} // namespace Presentation

#endif // PRESENTATION_AUTHORS_TABLE_MODEL_HPP

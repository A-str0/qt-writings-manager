#ifndef PRESENTATION_WRITINGS_TABLE_MODEL_HPP
#define PRESENTATION_WRITINGS_TABLE_MODEL_HPP

#include <QAbstractTableModel>
#include <QList>
#include <QMap>

#include "domain/writing.hpp"

namespace Presentation {

class WritingsTableModel final : public QAbstractTableModel
{
    Q_OBJECT

public:
    using QAbstractTableModel::QAbstractTableModel;

    int rowCount(const QModelIndex &parent = {}) const override
    {
        return parent.isValid() ? 0 : _writings.size();
    }

    int columnCount(const QModelIndex &parent = {}) const override
    {
        return parent.isValid() ? 0 : 4;
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override
    {
        if (!index.isValid() || index.row() >= _writings.size())
            return {};

        const auto &w = _writings[index.row()];

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
            case 0: return w.m_title;
            case 1: return _authorNames.value(w.m_authorId, QStringLiteral("—"));
            case 2: return w.m_description;
            case 3: return w.m_id.toString(QUuid::WithoutBraces);
            }
        }

        if (role == Qt::UserRole)
            return w.m_id;

        return {};
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override
    {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
            return {};

        switch (section) {
        case 0: return QStringLiteral("Название");
        case 1: return QStringLiteral("Автор");
        case 2: return QStringLiteral("Описание");
        case 3: return QStringLiteral("ID");
        }
        return {};
    }

    void setWritings(const QList<Domain::Writing> &writings, const QMap<QUuid, QString> &authorNames)
    {
        beginResetModel();
        _writings = writings;
        _authorNames = authorNames;
        endResetModel();
    }

    const Domain::Writing &writingAt(int row) const { return _writings[row]; }

    int rowByWritingId(const QUuid &id) const
    {
        for (int i = 0; i < _writings.size(); ++i)
            if (_writings[i].m_id == id) return i;
        return -1;
    }

private:
    QList<Domain::Writing> _writings;
    QMap<QUuid, QString> _authorNames;
};

} // namespace Presentation

#endif // PRESENTATION_WRITINGS_TABLE_MODEL_HPP

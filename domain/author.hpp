#ifndef DOMAIN_AUTHOR_HPP
#define DOMAIN_AUTHOR_HPP

#include <QString>
#include <QUuid>

namespace Domain {

class Author final
{
public:
    Author() = default;
    Author(QUuid id, QString name);

    QString validationError() const;

    QUuid m_id;
    QString m_name;
};

} // namespace Domain

#endif // DOMAIN_AUTHOR_HPP

#ifndef DOMAIN_WRITING_HPP
#define DOMAIN_WRITING_HPP

#include <QString>
#include <QUuid>

namespace Domain {

class Writing final
{
public:
    Writing() = default;
    Writing(QUuid id, QUuid authorId, QString title, QString description);

    QString validationError() const;

    QUuid m_id;
    QUuid m_authorId;
    QString m_title;
    QString m_description;
};

} // namespace Domain

#endif // DOMAIN_WRITING_HPP

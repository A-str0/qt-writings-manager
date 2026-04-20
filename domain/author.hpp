#ifndef DOMAIN_AUTHOR_HPP
#define DOMAIN_AUTHOR_HPP

#include <QString>

namespace Domain {

class Author final
{
public:
    Author() = default;
    Author(QString id, QString name, int birthYear, QString country);

    const QString &id() const;
    const QString &name() const;
    int birthYear() const;
    const QString &country() const;

    Author normalized() const;
    Author withId(QString id) const;
    QString validationError() const;

private:
    QString m_id;
    QString m_name;
    int m_birthYear = 0;
    QString m_country;
};

} // namespace Domain

#endif // DOMAIN_AUTHOR_HPP

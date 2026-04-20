#ifndef DOMAIN_WRITING_HPP
#define DOMAIN_WRITING_HPP

#include <QString>

namespace Domain {

class Writing final
{
public:
    Writing() = default;
    Writing(QString title, QString author, QString genre, int publicationYear, QString description);

    const QString &title() const;
    const QString &author() const;
    const QString &genre() const;
    int publicationYear() const;
    const QString &description() const;

    Writing normalized() const;
    QString validationError() const;

private:
    QString m_title;
    QString m_author;
    QString m_genre;
    int m_publicationYear = 0;
    QString m_description;
};

} // namespace Domain

#endif // DOMAIN_WRITING_HPP

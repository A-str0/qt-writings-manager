#ifndef DOMAIN_WRITING_HPP
#define DOMAIN_WRITING_HPP

#include <QString>

namespace Domain {

class Writing final
{
public:
    Writing() = default;
    Writing(
        QString id,
        QString title,
        QString authorId,
        QString genre,
        int publicationYear,
        QString description);

    const QString &id() const;

    const QString &title() const;
    const QString &authorId() const;
    const QString &genre() const;
    int publicationYear() const;
    const QString &description() const;

    Writing normalized() const;
    Writing withId(QString id) const;
    QString validationError() const;

private:
    QString m_id;
    QString m_title;
    QString m_authorId;
    QString m_genre;
    int m_publicationYear = 0;
    QString m_description;
};

} // namespace Domain

#endif // DOMAIN_WRITING_HPP

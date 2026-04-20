#ifndef PRESENTATION_MAIN_WINDOW_HPP
#define PRESENTATION_MAIN_WINDOW_HPP

#include <QMainWindow>

#include "application/authors_service.hpp"
#include "application/writings_service.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace Presentation {

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(
        Application::AuthorsService &authorsService,
        Application::WritingsService &writingsService,
        QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onLoadAuthorsClicked();
    void onSaveAuthorsClicked();
    void onAddAuthorClicked();
    void onUpdateAuthorClicked();
    void onDeleteAuthorClicked();
    void onClearAuthorClicked();

    void onLoadWritingsClicked();
    void onSaveWritingsClicked();
    void onAddWritingClicked();
    void onUpdateWritingClicked();
    void onDeleteWritingClicked();
    void onClearWritingClicked();

    void onAuthorTableSelectionChanged();
    void onWritingTableSelectionChanged();

private:
    void refreshAuthorsTable();
    void refreshWritingsTable();
    void refreshAuthorCombo(const QString &preferredAuthorId = QString());

    void clearAuthorForm();
    void clearWritingForm();

    void fillAuthorForm(const Domain::Author &author);
    void fillWritingForm(const Domain::Writing &writing);

    bool buildAuthorFromForm(Domain::Author &author);
    bool buildWritingFromForm(Domain::Writing &writing);

    QString authorNameById(const QString &authorId) const;
    void showStatus(const QString &message);
    void updateActionButtons();

    Application::AuthorsService &m_authorsService;
    Application::WritingsService &m_writingsService;
    Ui::MainWindow *m_ui;
    QString m_selectedAuthorId;
    QString m_selectedWritingId;
};

} // namespace Presentation

#endif // PRESENTATION_MAIN_WINDOW_HPP

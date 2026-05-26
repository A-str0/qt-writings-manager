#ifndef PRESENTATION_MAIN_WINDOW_HPP
#define PRESENTATION_MAIN_WINDOW_HPP

#include <QMainWindow>

#include "application/authors_service.hpp"
#include "application/writings_service.hpp"
#include "presentation/authors_table_model.hpp"
#include "presentation/writings_table_model.hpp"

class QComboBox;
class QLineEdit;
class QPlainTextEdit;
class QPushButton;
class QTableView;
class QWidget;

namespace Presentation {

class MainWindow final : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(
        Application::AuthorsService &authorsService,
        Application::WritingsService &writingsService,
        QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    void buildUi();
    QWidget *createAuthorsSection();
    QWidget *createWritingsSection();
    void configureTableView(QTableView *tableView) const;
    void connectSignals();

    void onLoadAuthorsClicked();
    void onSaveAuthorsClicked();
    void onAddAuthorClicked();
    void onUpdateAuthorClicked();
    void onDeleteAuthorClicked();
    void onClearAuthorClicked();
    void onAuthorSelectionChanged();

    void onLoadWritingsClicked();
    void onSaveWritingsClicked();
    void onAddWritingClicked();
    void onUpdateWritingClicked();
    void onDeleteWritingClicked();
    void onClearWritingClicked();
    void onWritingSelectionChanged();

    void refreshAuthorsTable(const QUuid &preferredAuthorId = {});
    void refreshWritingsTable(const QUuid &preferredWritingId = {});
    void refreshAuthorCombo(const QUuid &preferredAuthorId = {});

    void clearAuthorForm();
    void clearWritingForm();

    QUuid currentAuthorId() const;
    QUuid currentWritingId() const;
    void selectRowById(QTableView *tableView, const QAbstractItemModel &model, const QUuid &id) const;

    bool showResult(const Application::OperationResult &result);
    void updateButtons();
    void showStatus(const QString &message);

    Application::AuthorsService &_authorsService;
    Application::WritingsService &_writingsService;

    AuthorsTableModel _authorsTableModel;
    WritingsTableModel _writingsTableModel;

    QTableView *_authorsTableView = nullptr;
    QTableView *_writingsTableView = nullptr;

    QLineEdit *_authorNameEdit = nullptr;
    QLineEdit *_writingTitleEdit = nullptr;
    QPlainTextEdit *_writingDescriptionEdit = nullptr;
    QComboBox *_writingAuthorCombo = nullptr;

    QPushButton *_authorAddButton = nullptr;
    QPushButton *_authorUpdateButton = nullptr;
    QPushButton *_authorDeleteButton = nullptr;
    QPushButton *_authorClearButton = nullptr;
    QPushButton *_authorsLoadButton = nullptr;
    QPushButton *_authorsSaveButton = nullptr;

    QPushButton *_writingAddButton = nullptr;
    QPushButton *_writingUpdateButton = nullptr;
    QPushButton *_writingDeleteButton = nullptr;
    QPushButton *_writingClearButton = nullptr;
    QPushButton *_writingsLoadButton = nullptr;
    QPushButton *_writingsSaveButton = nullptr;
};

} // namespace Presentation

#endif // PRESENTATION_MAIN_WINDOW_HPP

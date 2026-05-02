#include "presentation/main_window.hpp"

#include <QAbstractItemView>
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSplitter>
#include <QStandardItem>
#include <QStatusBar>
#include <QTableView>
#include <QVBoxLayout>
#include <QWidget>

namespace {

constexpr int kIdRole = Qt::UserRole + 1;
constexpr int kAuthorIdRole = Qt::UserRole + 2;
const QString kCsvFilter = QStringLiteral("CSV files (*.csv);;All files (*)");

QString uuidToText(const QUuid &id)
{
    return id.toString(QUuid::WithoutBraces);
}

QStandardItem *makeReadOnlyItem(const QString &text)
{
    auto *item = new QStandardItem(text);
    item->setEditable(false);
    return item;
}

QString openCsvFile(QWidget *parent, const QString &title)
{
    return QFileDialog::getOpenFileName(parent, title, QString(), kCsvFilter);
}

QString saveCsvFile(QWidget *parent, const QString &title)
{
    QString filePath = QFileDialog::getSaveFileName(parent, title, QString(), kCsvFilter);
    if (!filePath.isEmpty() && !filePath.endsWith(QStringLiteral(".csv"), Qt::CaseInsensitive)) {
        filePath += QStringLiteral(".csv");
    }

    return filePath;
}

void clearModel(QStandardItemModel &model)
{
    if (model.rowCount() > 0) {
        model.removeRows(0, model.rowCount());
    }
}

void clearSelection(QTableView *tableView)
{
    tableView->clearSelection();
    tableView->setCurrentIndex(QModelIndex());
}

} // namespace

namespace Presentation {

MainWindow::MainWindow(
    Application::AuthorsService &authorsService,
    Application::WritingsService &writingsService,
    QWidget *parent)
    : QMainWindow(parent)
    , _authorsService(authorsService)
    , _writingsService(writingsService)
{
    buildUi();
    connectSignals();

    refreshAuthorsTable();
    refreshAuthorCombo();
    refreshWritingsTable();
    clearAuthorForm();
    clearWritingForm();

    setWindowTitle(QStringLiteral("Каталог литературы"));
    resize(1280, 820);
}

void MainWindow::buildUi()
{
    auto *centralWidget = new QWidget(this);
    auto *rootLayout = new QVBoxLayout(centralWidget);
    auto *splitter = new QSplitter(Qt::Vertical, centralWidget);

    splitter->addWidget(createAuthorsSection());
    splitter->addWidget(createWritingsSection());
    splitter->setChildrenCollapsible(false);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);

    rootLayout->addWidget(splitter);
    setCentralWidget(centralWidget);
    statusBar()->setSizeGripEnabled(false);

    _authorsModel.setColumnCount(2);
    _authorsModel.setHorizontalHeaderLabels({QStringLiteral("Имя автора"), QStringLiteral("ID")});
    _authorsTableView->setModel(&_authorsModel);
    _authorsTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    _authorsTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    _writingsModel.setColumnCount(4);
    _writingsModel.setHorizontalHeaderLabels(
        {QStringLiteral("Название"), QStringLiteral("Автор"), QStringLiteral("Описание"), QStringLiteral("ID")});
    _writingsTableView->setModel(&_writingsModel);
    _writingsTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    _writingsTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    _writingsTableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    _writingsTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
}

QWidget *MainWindow::createAuthorsSection()
{
    auto *groupBox = new QGroupBox(QStringLiteral("Авторы"), this);
    auto *layout = new QVBoxLayout(groupBox);
    auto *fileButtonsLayout = new QHBoxLayout();
    auto *contentLayout = new QHBoxLayout();
    auto *formWidget = new QWidget(groupBox);
    auto *formLayout = new QFormLayout();
    auto *formButtonsLayout = new QHBoxLayout();

    _authorsLoadButton = new QPushButton(QStringLiteral("Загрузить CSV"), groupBox);
    _authorsSaveButton = new QPushButton(QStringLiteral("Сохранить CSV"), groupBox);
    fileButtonsLayout->addWidget(_authorsLoadButton);
    fileButtonsLayout->addWidget(_authorsSaveButton);
    fileButtonsLayout->addStretch();

    _authorsTableView = new QTableView(groupBox);
    configureTableView(_authorsTableView);
    contentLayout->addWidget(_authorsTableView, 2);

    _authorNameEdit = new QLineEdit(formWidget);
    formLayout->addRow(QStringLiteral("Имя"), _authorNameEdit);

    _authorAddButton = new QPushButton(QStringLiteral("Добавить"), formWidget);
    _authorUpdateButton = new QPushButton(QStringLiteral("Обновить"), formWidget);
    _authorDeleteButton = new QPushButton(QStringLiteral("Удалить"), formWidget);
    _authorClearButton = new QPushButton(QStringLiteral("Очистить"), formWidget);

    formButtonsLayout->addWidget(_authorAddButton);
    formButtonsLayout->addWidget(_authorUpdateButton);
    formButtonsLayout->addWidget(_authorDeleteButton);
    formButtonsLayout->addWidget(_authorClearButton);

    auto *formRootLayout = new QVBoxLayout(formWidget);
    formRootLayout->addLayout(formLayout);
    formRootLayout->addStretch();
    formRootLayout->addLayout(formButtonsLayout);

    contentLayout->addWidget(formWidget, 1);

    layout->addLayout(fileButtonsLayout);
    layout->addLayout(contentLayout);
    return groupBox;
}

QWidget *MainWindow::createWritingsSection()
{
    auto *groupBox = new QGroupBox(QStringLiteral("Произведения"), this);
    auto *layout = new QVBoxLayout(groupBox);
    auto *fileButtonsLayout = new QHBoxLayout();
    auto *contentLayout = new QHBoxLayout();
    auto *formWidget = new QWidget(groupBox);
    auto *formLayout = new QFormLayout();
    auto *formButtonsLayout = new QHBoxLayout();

    _writingsLoadButton = new QPushButton(QStringLiteral("Загрузить CSV"), groupBox);
    _writingsSaveButton = new QPushButton(QStringLiteral("Сохранить CSV"), groupBox);
    fileButtonsLayout->addWidget(_writingsLoadButton);
    fileButtonsLayout->addWidget(_writingsSaveButton);
    fileButtonsLayout->addStretch();

    _writingsTableView = new QTableView(groupBox);
    configureTableView(_writingsTableView);
    contentLayout->addWidget(_writingsTableView, 2);

    _writingTitleEdit = new QLineEdit(formWidget);
    _writingAuthorCombo = new QComboBox(formWidget);
    _writingDescriptionEdit = new QPlainTextEdit(formWidget);
    _writingDescriptionEdit->setMinimumHeight(140);

    formLayout->addRow(QStringLiteral("Название"), _writingTitleEdit);
    formLayout->addRow(QStringLiteral("Автор"), _writingAuthorCombo);
    formLayout->addRow(QStringLiteral("Описание"), _writingDescriptionEdit);

    _writingAddButton = new QPushButton(QStringLiteral("Добавить"), formWidget);
    _writingUpdateButton = new QPushButton(QStringLiteral("Обновить"), formWidget);
    _writingDeleteButton = new QPushButton(QStringLiteral("Удалить"), formWidget);
    _writingClearButton = new QPushButton(QStringLiteral("Очистить"), formWidget);

    formButtonsLayout->addWidget(_writingAddButton);
    formButtonsLayout->addWidget(_writingUpdateButton);
    formButtonsLayout->addWidget(_writingDeleteButton);
    formButtonsLayout->addWidget(_writingClearButton);

    auto *formRootLayout = new QVBoxLayout(formWidget);
    formRootLayout->addLayout(formLayout);
    formRootLayout->addStretch();
    formRootLayout->addLayout(formButtonsLayout);

    contentLayout->addWidget(formWidget, 1);

    layout->addLayout(fileButtonsLayout);
    layout->addLayout(contentLayout);
    return groupBox;
}

void MainWindow::configureTableView(QTableView *tableView) const
{
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setAlternatingRowColors(true);
    tableView->setWordWrap(false);
    tableView->verticalHeader()->setVisible(false);
    tableView->horizontalHeader()->setHighlightSections(false);
}

void MainWindow::connectSignals()
{
    connect(_authorsLoadButton, &QPushButton::clicked, this, &MainWindow::onLoadAuthorsClicked);
    connect(_authorsSaveButton, &QPushButton::clicked, this, &MainWindow::onSaveAuthorsClicked);
    connect(_authorAddButton, &QPushButton::clicked, this, &MainWindow::onAddAuthorClicked);
    connect(_authorUpdateButton, &QPushButton::clicked, this, &MainWindow::onUpdateAuthorClicked);
    connect(_authorDeleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteAuthorClicked);
    connect(_authorClearButton, &QPushButton::clicked, this, &MainWindow::onClearAuthorClicked);

    connect(_writingsLoadButton, &QPushButton::clicked, this, &MainWindow::onLoadWritingsClicked);
    connect(_writingsSaveButton, &QPushButton::clicked, this, &MainWindow::onSaveWritingsClicked);
    connect(_writingAddButton, &QPushButton::clicked, this, &MainWindow::onAddWritingClicked);
    connect(_writingUpdateButton, &QPushButton::clicked, this, &MainWindow::onUpdateWritingClicked);
    connect(_writingDeleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteWritingClicked);
    connect(_writingClearButton, &QPushButton::clicked, this, &MainWindow::onClearWritingClicked);

    connect(
        _authorsTableView->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        &MainWindow::onAuthorSelectionChanged);
    connect(
        _writingsTableView->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        &MainWindow::onWritingSelectionChanged);
}

void MainWindow::onLoadAuthorsClicked()
{
    const QString filePath = openCsvFile(this, QStringLiteral("Загрузить авторов из CSV"));
    if (filePath.isEmpty()) {
        return;
    }

    if (!showResult(_authorsService.loadFromFile(filePath))) {
        return;
    }

    refreshAuthorsTable();
    refreshAuthorCombo();
    refreshWritingsTable();
    clearAuthorForm();
    clearWritingForm();
}

void MainWindow::onSaveAuthorsClicked()
{
    const QString filePath = saveCsvFile(this, QStringLiteral("Сохранить авторов в CSV"));
    if (filePath.isEmpty()) {
        return;
    }

    showResult(_authorsService.saveToFile(filePath));
}

void MainWindow::onAddAuthorClicked()
{
    if (!showResult(_authorsService.addAuthor(authorFromForm()))) {
        return;
    }

    refreshAuthorsTable();
    refreshAuthorCombo();
    clearAuthorForm();
}

void MainWindow::onUpdateAuthorClicked()
{
    const QUuid authorId = currentAuthorId();
    if (authorId.isNull()) {
        showStatus(QStringLiteral("Сначала выберите автора для редактирования."));
        return;
    }

    const QUuid writingId = currentWritingId();
    if (!showResult(_authorsService.updateAuthor(authorId, authorFromForm()))) {
        return;
    }

    refreshAuthorsTable(authorId);
    refreshAuthorCombo();
    refreshWritingsTable(writingId);
}

void MainWindow::onDeleteAuthorClicked()
{
    const QUuid authorId = currentAuthorId();
    if (authorId.isNull()) {
        showStatus(QStringLiteral("Сначала выберите автора для удаления."));
        return;
    }

    if (!showResult(_authorsService.removeAuthor(authorId))) {
        return;
    }

    refreshAuthorsTable();
    refreshAuthorCombo();
    clearAuthorForm();
}

void MainWindow::onClearAuthorClicked()
{
    clearAuthorForm();
    showStatus(QString());
}

void MainWindow::onAuthorSelectionChanged()
{
    const QUuid authorId = currentAuthorId();
    if (authorId.isNull()) {
        updateButtons();
        return;
    }

    const QModelIndex currentIndex = _authorsTableView->currentIndex();
    fillAuthorForm(Domain::Author(
        authorId, _authorsModel.index(currentIndex.row(), 0).data().toString()));
    updateButtons();
}

void MainWindow::onLoadWritingsClicked()
{
    const QString filePath = openCsvFile(this, QStringLiteral("Загрузить произведения из CSV"));
    if (filePath.isEmpty()) {
        return;
    }

    if (!showResult(_writingsService.loadFromFile(filePath))) {
        return;
    }

    refreshWritingsTable();
    clearWritingForm();
}

void MainWindow::onSaveWritingsClicked()
{
    const QString filePath = saveCsvFile(this, QStringLiteral("Сохранить произведения в CSV"));
    if (filePath.isEmpty()) {
        return;
    }

    showResult(_writingsService.saveToFile(filePath));
}

void MainWindow::onAddWritingClicked()
{
    Domain::Writing writing;
    if (!writingFromForm(writing)) {
        return;
    }

    if (!showResult(_writingsService.addWriting(writing))) {
        return;
    }

    refreshWritingsTable();
    clearWritingForm();
}

void MainWindow::onUpdateWritingClicked()
{
    const QUuid writingId = currentWritingId();
    if (writingId.isNull()) {
        showStatus(QStringLiteral("Сначала выберите произведение для редактирования."));
        return;
    }

    Domain::Writing writing;
    if (!writingFromForm(writing)) {
        return;
    }

    if (!showResult(_writingsService.updateWriting(writingId, writing))) {
        return;
    }

    refreshWritingsTable(writingId);
}

void MainWindow::onDeleteWritingClicked()
{
    const QUuid writingId = currentWritingId();
    if (writingId.isNull()) {
        showStatus(QStringLiteral("Сначала выберите произведение для удаления."));
        return;
    }

    if (!showResult(_writingsService.removeWriting(writingId))) {
        return;
    }

    refreshWritingsTable();
    clearWritingForm();
}

void MainWindow::onClearWritingClicked()
{
    clearWritingForm();
    showStatus(QString());
}

void MainWindow::onWritingSelectionChanged()
{
    const QUuid writingId = currentWritingId();
    if (writingId.isNull()) {
        updateButtons();
        return;
    }

    const QModelIndex currentIndex = _writingsTableView->currentIndex();
    const QModelIndex titleIndex = _writingsModel.index(currentIndex.row(), 0);

    fillWritingForm(Domain::Writing(
        writingId,
        titleIndex.data(kAuthorIdRole).toUuid(),
        titleIndex.data().toString(),
        _writingsModel.index(currentIndex.row(), 2).data().toString()));
    updateButtons();
}

void MainWindow::refreshAuthorsTable(const QUuid &preferredAuthorId)
{
    clearModel(_authorsModel);

    const QList<Domain::Author> authors = _authorsService.listAuthors();
    for (const Domain::Author &author : authors) {
        appendAuthorRow(author);
    }

    selectRowById(_authorsTableView, _authorsModel, preferredAuthorId);
    updateButtons();
}

void MainWindow::refreshWritingsTable(const QUuid &preferredWritingId)
{
    clearModel(_writingsModel);

    const QList<Domain::Writing> writings = _writingsService.listWritings();
    for (const Domain::Writing &writing : writings) {
        appendWritingRow(writing);
    }

    selectRowById(_writingsTableView, _writingsModel, preferredWritingId);
    updateButtons();
}

void MainWindow::refreshAuthorCombo(const QUuid &preferredAuthorId)
{
    QUuid authorIdToSelect = preferredAuthorId;
    if (authorIdToSelect.isNull()) {
        authorIdToSelect = _writingAuthorCombo->currentData().toUuid();
    }

    _writingAuthorCombo->clear();

    const QList<Domain::Author> authors = _authorsService.listAuthors();
    for (const Domain::Author &author : authors) {
        _writingAuthorCombo->addItem(author.m_name, author.m_id);
    }

    if (!authorIdToSelect.isNull()) {
        const int index = _writingAuthorCombo->findData(authorIdToSelect);
        if (index >= 0) {
            _writingAuthorCombo->setCurrentIndex(index);
        }
    }

    if (_writingAuthorCombo->count() > 0 && _writingAuthorCombo->currentIndex() < 0) {
        _writingAuthorCombo->setCurrentIndex(0);
    }

    updateButtons();
}

void MainWindow::appendAuthorRow(const Domain::Author &author)
{
    auto *nameItem = makeReadOnlyItem(author.m_name);
    nameItem->setData(author.m_id, kIdRole);

    _authorsModel.appendRow({nameItem, makeReadOnlyItem(uuidToText(author.m_id))});
}

void MainWindow::appendWritingRow(const Domain::Writing &writing)
{
    auto *titleItem = makeReadOnlyItem(writing.m_title);
    titleItem->setData(writing.m_id, kIdRole);
    titleItem->setData(writing.m_authorId, kAuthorIdRole);

    _writingsModel.appendRow(
        {titleItem,
         makeReadOnlyItem(authorNameById(writing.m_authorId)),
         makeReadOnlyItem(writing.m_description),
         makeReadOnlyItem(uuidToText(writing.m_id))});
}

Domain::Author MainWindow::authorFromForm() const
{
    return Domain::Author(QUuid(), _authorNameEdit->text().trimmed());
}

bool MainWindow::writingFromForm(Domain::Writing &writing)
{
    const QUuid authorId = _writingAuthorCombo->currentData().toUuid();
    if (authorId.isNull()) {
        showStatus(QStringLiteral("Сначала добавьте хотя бы одного автора."));
        return false;
    }

    writing = Domain::Writing(
        QUuid(),
        authorId,
        _writingTitleEdit->text().trimmed(),
        _writingDescriptionEdit->toPlainText().trimmed());
    return true;
}

void MainWindow::fillAuthorForm(const Domain::Author &author)
{
    _authorNameEdit->setText(author.m_name);
}

void MainWindow::fillWritingForm(const Domain::Writing &writing)
{
    _writingTitleEdit->setText(writing.m_title);
    _writingDescriptionEdit->setPlainText(writing.m_description);

    const int index = _writingAuthorCombo->findData(writing.m_authorId);
    if (index >= 0) {
        _writingAuthorCombo->setCurrentIndex(index);
    }
}

void MainWindow::clearAuthorForm()
{
    _authorNameEdit->clear();
    clearSelection(_authorsTableView);
    updateButtons();
}

void MainWindow::clearWritingForm()
{
    _writingTitleEdit->clear();
    _writingDescriptionEdit->clear();
    clearSelection(_writingsTableView);

    if (_writingAuthorCombo->count() > 0) {
        _writingAuthorCombo->setCurrentIndex(0);
    }

    updateButtons();
}

QUuid MainWindow::currentAuthorId() const
{
    const QModelIndex currentIndex = _authorsTableView->currentIndex();
    if (!currentIndex.isValid()) {
        return {};
    }

    return _authorsModel.index(currentIndex.row(), 0).data(kIdRole).toUuid();
}

QUuid MainWindow::currentWritingId() const
{
    const QModelIndex currentIndex = _writingsTableView->currentIndex();
    if (!currentIndex.isValid()) {
        return {};
    }

    return _writingsModel.index(currentIndex.row(), 0).data(kIdRole).toUuid();
}

void MainWindow::selectRowById(
    QTableView *tableView, const QStandardItemModel &model, const QUuid &id) const
{
    if (id.isNull()) {
        tableView->clearSelection();
        tableView->setCurrentIndex(QModelIndex());
        return;
    }

    for (int row = 0; row < model.rowCount(); ++row) {
        const QModelIndex index = model.index(row, 0);
        if (index.data(kIdRole).toUuid() == id) {
            tableView->setCurrentIndex(index);
            tableView->selectRow(row);
            tableView->scrollTo(index);
            return;
        }
    }

    tableView->clearSelection();
    tableView->setCurrentIndex(QModelIndex());
}

QString MainWindow::authorNameById(const QUuid &authorId) const
{
    const QList<Domain::Author> authors = _authorsService.listAuthors();
    for (const Domain::Author &author : authors) {
        if (author.m_id == authorId) {
            return author.m_name;
        }
    }

    return QStringLiteral("—");
}

bool MainWindow::showResult(const Application::OperationResult &result)
{
    showStatus(result.m_message);
    return result.m_ok;
}

void MainWindow::updateButtons()
{
    const bool hasAuthorSelection = !currentAuthorId().isNull();
    const bool hasWritingSelection = !currentWritingId().isNull();
    const bool hasAuthors = _writingAuthorCombo->count() > 0;

    _authorUpdateButton->setEnabled(hasAuthorSelection);
    _authorDeleteButton->setEnabled(hasAuthorSelection);

    _writingAuthorCombo->setEnabled(hasAuthors);
    _writingAddButton->setEnabled(hasAuthors);
    _writingUpdateButton->setEnabled(hasWritingSelection);
    _writingDeleteButton->setEnabled(hasWritingSelection);
}

void MainWindow::showStatus(const QString &message)
{
    statusBar()->showMessage(message);
}

} // namespace Presentation

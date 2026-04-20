#include <QApplication>

#include "application/authors_service.hpp"
#include "application/writings_service.hpp"
#include "infrastructure/csv_authors_file_storage.hpp"
#include "infrastructure/csv_writings_file_storage.hpp"
#include "infrastructure/in_memory_authors_repository.hpp"
#include "infrastructure/in_memory_writings_repository.hpp"
#include "presentation/main_window.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("Literature Catalog");
    QApplication::setOrganizationName("course_part1");

    Infrastructure::InMemoryAuthorsRepository authorsRepository;
    Infrastructure::InMemoryWritingsRepository writingsRepository;
    Infrastructure::CsvAuthorsFileStorage authorsFileStorage;
    Infrastructure::CsvWritingsFileStorage writingsFileStorage;
    Application::AuthorsService authorsService(
        authorsRepository, writingsRepository, authorsFileStorage);
    Application::WritingsService writingsService(
        writingsRepository, authorsRepository, writingsFileStorage);
    Presentation::MainWindow window(authorsService, writingsService);
    window.show();

    return app.exec();
}

#include <QApplication>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWebEngineView>
#include <QUrl>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QWidget window;
    QVBoxLayout *layout = new QVBoxLayout(&window);

    // Search/Address Bar
    QLineEdit *searchBar = new QLineEdit();
    searchBar->setPlaceholderText("Search or enter URL...");
    layout->addWidget(searchBar);

    // Browser Engine
    QWebEngineView *view = new QWebEngineView();
    view->setUrl(QUrl("https://www.google.com"));
    layout->addWidget(view);

    // Search Logic
    QObject::connect(searchBar, &QLineEdit::returnPressed, [=]() {
        QString text = searchBar->text();
        if (text.contains(".") && !text.contains(" ")) {
            view->setUrl(QUrl::fromUserInput(text)); // Open URL
        } else {
            view->setUrl(QUrl("https://www.google.com/search?q=" + text)); // Search
        }
    });

    window.setWindowTitle("ASK Browser - Alpha");
    window.resize(1024, 768);
    window.show();
    return app.exec();
}
sudo apt update && sudo apt install qtbase5-dev qtwebengine5-dev libqt5webengine5 libqt5webenginewidgets5
g++ ask_browser.cpp -o ask_browser -fPIC $(pkg-config --cflags --libs Qt5Widgets Qt5WebEngineWidgets)

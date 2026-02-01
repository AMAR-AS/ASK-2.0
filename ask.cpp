#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QWebEngineView>
#include <QFrame>

class AskBrowser : public QWidget {
public:
    AskBrowser() {
        QHBoxLayout *rootLayout = new QHBoxLayout(this);
        rootLayout->setContentsMargins(0, 0, 0, 0);
        rootLayout->setSpacing(0);

        // --- 1. SIDEBAR (Sectors) ---
        QFrame *sidebar = new QFrame();
        sidebar->setFixedWidth(60);
        sidebar->setStyleSheet("background-color: #121212; border-right: 1px solid #333;");
        QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
        
        QPushButton *personalBtn = new QPushButton("P");
        QPushButton *workBtn = new QPushButton("W");
        QPushButton *settingsBtn = new QPushButton("⚙️"); // Settings Button
        
        sideLayout->addWidget(personalBtn);
        sideLayout->addWidget(workBtn);
        sideLayout->addStretch();
        sideLayout->addWidget(settingsBtn);
        rootLayout->addWidget(sidebar);

        // --- 2. MAIN CONTENT AREA ---
        QVBoxLayout *mainArea = new QVBoxLayout();
        
        // Top Control Bar (Search + New Tab)
        QHBoxLayout *topBar = new QHBoxLayout();
        QLineEdit *searchBar = new QLineEdit();
        QPushButton *newTabBtn = new QPushButton("+");
        newTabBtn->setFixedWidth(40);
        
        topBar->addWidget(searchBar);
        topBar->addWidget(newTabBtn);
        mainArea->addLayout(topBar);

        // Tab Widget
        tabs = new QTabWidget();
        tabs->setTabsClosable(true);
        mainArea->addWidget(tabs);
        
        rootLayout->addLayout(mainArea);

        // --- LOGIC ---
        // Add initial tab
        addNewTab("https://www.google.com");

        connect(newTabBtn, &QPushButton::clicked, [this]() { addNewTab("https://www.google.com"); });
        connect(tabs, &QTabWidget::tabCloseRequested, [this](int index) { 
            if(tabs->count() > 1) tabs->removeTab(index); 
        });
        
        // Search logic for CURRENT tab
        connect(searchBar, &QLineEdit::returnPressed, [=]() {
            QWebEngineView *currentView = qobject_cast<QWebEngineView*>(tabs->currentWidget());
            if (currentView) currentView->setUrl(QUrl::fromUserInput(searchBar->text()));
        });
    }

private:
    QTabWidget *tabs;
    void addNewTab(const QString &url) {
        QWebEngineView *view = new QWebEngineView();
        view->setUrl(QUrl(url));
        int index = tabs->addTab(view, "New Tab");
        tabs->setCurrentIndex(index);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyleSheet("QWidget { background-color: #1e1e1e; color: white; }");
    AskBrowser browser;
    browser.showMaximized();
    return app.exec();
}

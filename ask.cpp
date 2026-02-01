#include <QApplication>
#include <QWebEngineView>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QFrame>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDateTime>
#include <QClipboard>
#include <QFontDatabase>
#include <QMessageBox>
#include <QMap>

// We use a Class structure now for better management
class AskBrowser : public QWidget {
public:
    AskBrowser() {
        // --- 1. SETUP DATABASE (Local History) ---
        setupDatabase();

        QHBoxLayout *rootLayout = new QHBoxLayout(this);
        rootLayout->setContentsMargins(0, 0, 0, 0);

        // --- 2. SIDEBAR (Sectors) ---
        QFrame *sidebar = new QFrame();
        sidebar->setFixedWidth(60);
        sidebar->setStyleSheet("background-color: #121212; border-right: 1px solid #333;");
        QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
        
        QPushButton *aiBtn = new QPushButton("AI");
        QPushButton *workBtn = new QPushButton("W");
        QPushButton *settingsBtn = new QPushButton("⚙️");
        
        sideLayout->addWidget(aiBtn);
        sideLayout->addWidget(workBtn);
        sideLayout->addStretch();
        sideLayout->addWidget(settingsBtn);
        rootLayout->addWidget(sidebar);

        // --- 3. MAIN AREA ---
        QVBoxLayout *mainArea = new QVBoxLayout();
        QHBoxLayout *topBar = new QHBoxLayout();

        // Search Engine Selector
        engineSelector = new QComboBox();
        engineSelector->addItems({"ASK", "DuckDuckGo", "Google", "Bing"});
        // Define Search Engine URLs
        searchMaps["ASK"] = "https://ask-search.example.com/?q="; // Placeholder
        searchMaps["DuckDuckGo"] = "https://duckduckgo.com/?q=";
        searchMaps["Google"] = "https://www.google.com/search?q=";
        searchMaps["Bing"] = "https://www.bing.com/search?q=";

        searchBar = new QLineEdit();
        searchBar->setPlaceholderText("Search or Enter URL...");

        // Productivity Tools
        QPushButton *snapBtn = new QPushButton("📸");
        QPushButton *clipBtn = new QPushButton("📋");
        QPushButton *newTabBtn = new QPushButton("+");
        newTabBtn->setFixedWidth(40);

        topBar->addWidget(engineSelector);
        topBar->addWidget(searchBar);
        topBar->addWidget(snapBtn);
        topBar->addWidget(clipBtn);
        topBar->addWidget(newTabBtn);
        mainArea->addLayout(topBar);

        // Tab Widget
        tabs = new QTabWidget();
        tabs->setTabsClosable(true);
        mainArea->addWidget(tabs);
        rootLayout->addLayout(mainArea);

        // --- 4. LOGIC CONNECTIONS ---
        addNewTab("https://duckduckgo.com"); // Start Private

        connect(newTabBtn, &QPushButton::clicked, [this]() { addNewTab("https://duckduckgo.com"); });
        
        connect(tabs, &QTabWidget::tabCloseRequested, [this](int index) {
            if (tabs->count() > 1) tabs->removeTab(index);
        });

        // Search Logic
        connect(searchBar, &QLineEdit::returnPressed, [this]() {
            QWebEngineView *view = currentView();
            if (!view) return;
            QString input = searchBar->text();
            if (input.contains(".") && !input.contains(" ")) {
                view->setUrl(QUrl::fromUserInput(input));
            } else {
                QString engine = engineSelector->currentText();
                view->setUrl(QUrl(searchMaps[engine] + input));
            }
        });

        // Screenshot Logic
        connect(snapBtn, &QPushButton::clicked, [this]() {
            if (QWebEngineView *view = currentView()) {
                QPixmap pix = view->grab();
                QString fileName = "ASK_Snap_" + QDateTime::currentDateTime().toString("hhmmss") + ".png";
                pix.save(fileName);
                QMessageBox::information(this, "Saved", "Screenshot Saved to: " + fileName);
            }
        });

        // Clipboard Logic
        connect(clipBtn, &QPushButton::clicked, [this]() {
            if (QWebEngineView *view = currentView()) 
                QApplication::clipboard()->setText(view->url().toString());
        });
        
        connect(aiBtn, &QPushButton::clicked, [this](){ addNewTab("https://gemini.google.com"); });
    }

private:
    QTabWidget *tabs;
    QComboBox *engineSelector;
    QLineEdit *searchBar;
    QMap<QString, QString> searchMaps;

    void setupDatabase() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("ask_history.db");
        if (db.open()) {
            QSqlQuery query;
            query.exec("CREATE TABLE IF NOT EXISTS history (id INTEGER PRIMARY KEY, url TEXT, time TEXT)");
        }
    }

    void addNewTab(const QString &url) {
        QWebEngineView *view = new QWebEngineView();
        view->setUrl(QUrl(url));
        
        // Auto-Save History
        connect(view, &QWebEngineView::urlChanged, [](const QUrl &u){
            QSqlQuery query;
            query.prepare("INSERT INTO history (url, time) VALUES (?, ?)");
            query.addBindValue(u.toString());
            query.addBindValue(QDateTime::currentDateTime().toString());
            query.exec();
        });

        int i = tabs->addTab(view, "New Tab");
        tabs->setCurrentIndex(i);
    }

    QWebEngineView* currentView() {
        return qobject_cast<QWebEngineView*>(tabs->currentWidget());
    }
};

// Include MOC if using qmake, otherwise handled by compiler usually
#include "ask_v4.moc" 

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    // Load Oxanium Font (Ensure the .ttf file is in the same folder)
    int fontId = QFontDatabase::addApplicationFont("./Oxanium-Regular.ttf");
    if (fontId != -1) {
        QFont oxanium(QFontDatabase::applicationFontFamilies(fontId).at(0));
        app.setFont(oxanium);
    }
    
    // Dark Theme
    app.setStyleSheet("QWidget { background-color: #1e1e1e; color: white; } "
                      "QLineEdit { background-color: #333; border: 1px solid #555; padding: 4px; color: white; }"
                      "QComboBox { background-color: #333; color: white; padding: 4px; }");

    AskBrowser browser;
    browser.showMaximized();
    return app.exec();
}

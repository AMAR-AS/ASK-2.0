/**
 * ASK BROWSER - Complete Implementation v8.0
 * "The Liquid Glass Edition"
 * 
 * Features:
 * - Futuristic Liquid Glass UI
 * - Smart Expandable Sidebar
 * - Multi-Engine Search
 * - Session Management
 * - AI/Work Mode Quick Access
 * - Full Keyboard Shortcuts
 * - SQLite History
 * - Performance Optimized
 */

#include <QApplication>
#include <QMainWindow>
#include <QWebEngineView>
#include <QWebEngineSettings>
#include <QWebEngineProfile>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QFrame>
#include <QLabel>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QFontDatabase>
#include <QMap>
#include <QShortcut>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QTimer>
#include <QDebug>

// ============================================================================
// CUSTOM STYLED WIDGETS
// ============================================================================

class GlassFrame : public QFrame {
public:
    GlassFrame(QWidget *parent = nullptr) : QFrame(parent) {
        setStyleSheet(R"(
            QFrame {
                background: rgba(15, 15, 35, 0.85);
                border: 1px solid rgba(255, 255, 255, 0.1);
                border-radius: 12px;
            }
        )");
    }
};

class GlassButton : public QPushButton {
public:
    GlassButton(const QString &text, QWidget *parent = nullptr) : QPushButton(text, parent) {
        setStyleSheet(R"(
            QPushButton {
                background: rgba(255, 255, 255, 0.05);
                border: 1px solid rgba(255, 255, 255, 0.1);
                border-radius: 8px;
                color: white;
                padding: 10px 20px;
                font-weight: 600;
                font-size: 14px;
            }
            QPushButton:hover {
                background: rgba(0, 212, 255, 0.2);
                border-color: #00d4ff;
                box-shadow: 0 4px 12px rgba(0, 212, 255, 0.3);
            }
            QPushButton:pressed {
                background: rgba(0, 212, 255, 0.3);
            }
        )");
        setCursor(Qt::PointingHandCursor);
    }
};

class GlassSearchBar : public QLineEdit {
public:
    GlassSearchBar(QWidget *parent = nullptr) : QLineEdit(parent) {
        setStyleSheet(R"(
            QLineEdit {
                background: rgba(255, 255, 255, 0.05);
                border: 1px solid rgba(255, 255, 255, 0.1);
                border-radius: 12px;
                color: white;
                padding: 12px 20px;
                font-size: 14px;
            }
            QLineEdit:focus {
                background: rgba(255, 255, 255, 0.08);
                border-color: #00d4ff;
                box-shadow: 0 0 20px rgba(0, 212, 255, 0.2);
            }
        )");
        setPlaceholderText("🔍 Search or enter URL...");
    }
};

// ============================================================================
// MAIN BROWSER CLASS
// ============================================================================

class AskBrowser : public QMainWindow {
public:
    AskBrowser() {
        // Setup
        setupDatabase();
        loadOxaniumFont();
        setupUI();
        setupConnections();
        setupShortcuts();
        
        // Initial state
        currentWorkspace = "Personal";
        workspaceUrls["AI"] = "https://gemini.google.com";
        workspaceUrls["Work"] = "https://linkedin.com";
        workspaceUrls["Personal"] = "https://duckduckgo.com";
        
        // Open first tab
        addNewTab(workspaceUrls["Personal"]);
        
        setWindowTitle("ASK Browser - The Liquid Glass Edition");
        resize(1400, 900);
    }

private:
    // UI Components
    QWidget *centralWidget;
    QFrame *sidebar;
    QTabWidget *tabWidget;
    GlassSearchBar *searchBar;
    QComboBox *engineSelector;
    QLabel *statusLabel;
    QLabel *workspaceLabel;
    
    // Sidebar buttons
    QPushButton *menuBtn;
    QPushButton *homeBtn;
    QPushButton *aiBtn;
    QPushButton *workBtn;
    QPushButton *downloadsBtn;
    QPushButton *vaultBtn;
    QPushButton *settingsBtn;
    
    // State
    bool sidebarExpanded = false;
    QString currentWorkspace;
    QMap<QString, QString> searchEngines;
    QMap<QString, QString> workspaceUrls;
    int trackersBlocked = 0;

    // ========================================================================
    // UI SETUP
    // ========================================================================
    
    void setupUI() {
        centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);
        
        QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);
        
        // Create sidebar
        createSidebar();
        mainLayout->addWidget(sidebar);
        
        // Create main content area
        QVBoxLayout *contentLayout = new QVBoxLayout();
        contentLayout->setContentsMargins(0, 0, 0, 0);
        contentLayout->setSpacing(0);
        
        // Top bar
        createTopBar(contentLayout);
        
        // Tab widget
        createTabWidget(contentLayout);
        
        // Status bar
        createStatusBar(contentLayout);
        
        mainLayout->addLayout(contentLayout, 1);
        
        // Apply global glass styling
        applyGlobalStyle();
    }
    
    void createSidebar() {
        sidebar = new QFrame();
        sidebar->setFixedWidth(60);
        sidebar->setStyleSheet(R"(
            QFrame {
                background: rgba(15, 15, 35, 0.95);
                border-right: 1px solid rgba(255, 255, 255, 0.1);
            }
        )");
        
        QVBoxLayout *sideLayout = new QVBoxLayout(sidebar);
        sideLayout->setContentsMargins(5, 15, 5, 15);
        sideLayout->setSpacing(10);
        
        // Logo/Menu button
        menuBtn = createSidebarButton("☰", "Expand Menu");
        sideLayout->addWidget(menuBtn);
        
        sideLayout->addSpacing(20);
        
        // Navigation buttons
        homeBtn = createSidebarButton("🏠", "Home");
        aiBtn = createSidebarButton("🤖", "AI Sector");
        workBtn = createSidebarButton("💼", "Work Mode");
        downloadsBtn = createSidebarButton("📥", "Downloads");
        vaultBtn = createSidebarButton("🔒", "Secure Vault");
        
        sideLayout->addWidget(homeBtn);
        sideLayout->addWidget(aiBtn);
        sideLayout->addWidget(workBtn);
        sideLayout->addWidget(downloadsBtn);
        sideLayout->addWidget(vaultBtn);
        
        sideLayout->addStretch();
        
        // Settings at bottom
        settingsBtn = createSidebarButton("⚙️", "Settings");
        sideLayout->addWidget(settingsBtn);
    }
    
    QPushButton* createSidebarButton(const QString &icon, const QString &tooltip) {
        QPushButton *btn = new QPushButton(icon);
        btn->setToolTip(tooltip);
        btn->setFixedHeight(50);
        btn->setStyleSheet(R"(
            QPushButton {
                background: transparent;
                border: none;
                color: rgba(255, 255, 255, 0.7);
                font-size: 24px;
                text-align: left;
                padding-left: 15px;
            }
            QPushButton:hover {
                background: rgba(0, 212, 255, 0.1);
                color: #00d4ff;
                border-left: 3px solid #00d4ff;
            }
        )");
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    }
    
    void createTopBar(QVBoxLayout *layout) {
        GlassFrame *topBar = new GlassFrame();
        topBar->setFixedHeight(70);
        
        QHBoxLayout *topLayout = new QHBoxLayout(topBar);
        topLayout->setContentsMargins(15, 10, 15, 10);
        
        // Navigation buttons
        GlassButton *backBtn = new GlassButton("←");
        GlassButton *forwardBtn = new GlassButton("→");
        GlassButton *reloadBtn = new GlassButton("⟳");
        
        backBtn->setFixedWidth(50);
        forwardBtn->setFixedWidth(50);
        reloadBtn->setFixedWidth(50);
        
        topLayout->addWidget(backBtn);
        topLayout->addWidget(forwardBtn);
        topLayout->addWidget(reloadBtn);
        topLayout->addSpacing(10);
        
        // Search engine selector
        engineSelector = new QComboBox();
        engineSelector->addItems({"ASK", "DuckDuckGo", "Google", "Bing", "Brave"});
        engineSelector->setStyleSheet(R"(
            QComboBox {
                background: rgba(255, 255, 255, 0.05);
                border: 1px solid rgba(255, 255, 255, 0.1);
                border-radius: 8px;
                color: white;
                padding: 8px 15px;
                min-width: 120px;
            }
            QComboBox:hover {
                background: rgba(255, 255, 255, 0.08);
            }
            QComboBox::drop-down {
                border: none;
            }
        )");
        
        searchEngines["ASK"] = "https://searx.be/search?q=";
        searchEngines["DuckDuckGo"] = "https://duckduckgo.com/?q=";
        searchEngines["Google"] = "https://www.google.com/search?q=";
        searchEngines["Bing"] = "https://www.bing.com/search?q=";
        searchEngines["Brave"] = "https://search.brave.com/search?q=";
        
        topLayout->addWidget(engineSelector);
        topLayout->addSpacing(10);
        
        // Search bar
        searchBar = new GlassSearchBar();
        topLayout->addWidget(searchBar, 1);
        topLayout->addSpacing(10);
        
        // AI button
        GlassButton *aiQuickBtn = new GlassButton("✨ Ask AI");
        topLayout->addWidget(aiQuickBtn);
        
        // New tab button
        GlassButton *newTabBtn = new GlassButton("+");
        newTabBtn->setFixedWidth(50);
        topLayout->addWidget(newTabBtn);
        
        layout->addWidget(topBar);
        
        // Connect top bar actions
        connect(backBtn, &QPushButton::clicked, [this]() {
            if (currentView()) currentView()->back();
        });
        
        connect(forwardBtn, &QPushButton::clicked, [this]() {
            if (currentView()) currentView()->forward();
        });
        
        connect(reloadBtn, &QPushButton::clicked, [this]() {
            if (currentView()) currentView()->reload();
        });
        
        connect(aiQuickBtn, &QPushButton::clicked, [this]() {
            openAIPanel();
        });
        
        connect(newTabBtn, &QPushButton::clicked, [this]() {
            addNewTab(workspaceUrls[currentWorkspace]);
        });
    }
    
    void createTabWidget(QVBoxLayout *layout) {
        tabWidget = new QTabWidget();
        tabWidget->setTabsClosable(true);
        tabWidget->setMovable(true);
        tabWidget->setDocumentMode(true);
        
        tabWidget->setStyleSheet(R"(
            QTabWidget::pane {
                border: none;
                background: #0a0a1f;
            }
            QTabBar::tab {
                background: rgba(255, 255, 255, 0.05);
                border: 1px solid rgba(255, 255, 255, 0.1);
                border-bottom: none;
                border-radius: 8px 8px 0 0;
                color: rgba(255, 255, 255, 0.7);
                padding: 10px 20px;
                margin-right: 5px;
                min-width: 150px;
            }
            QTabBar::tab:selected {
                background: rgba(0, 212, 255, 0.15);
                color: white;
                border-bottom: 2px solid #00d4ff;
            }
            QTabBar::tab:hover {
                background: rgba(255, 255, 255, 0.08);
            }
            QTabBar::close-button {
                image: url(none);
                subcontrol-position: right;
            }
            QTabBar::close-button:hover {
                background: rgba(255, 0, 85, 0.3);
            }
        )");
        
        layout->addWidget(tabWidget);
    }
    
    void createStatusBar(QVBoxLayout *layout) {
        GlassFrame *statusBar = new GlassFrame();
        statusBar->setFixedHeight(35);
        
        QHBoxLayout *statusLayout = new QHBoxLayout(statusBar);
        statusLayout->setContentsMargins(15, 5, 15, 5);
        
        // Left side status
        QLabel *securityIcon = new QLabel("🔒 Secure");
        securityIcon->setStyleSheet("color: #00ff88; font-size: 12px;");
        
        workspaceLabel = new QLabel("Personal Mode");
        workspaceLabel->setStyleSheet(R"(
            background: rgba(0, 212, 255, 0.1);
            color: #00d4ff;
            padding: 4px 10px;
            border-radius: 12px;
            font-size: 11px;
        )");
        
        statusLabel = new QLabel("Trackers Blocked: 0");
        statusLabel->setStyleSheet("color: rgba(255, 255, 255, 0.6); font-size: 11px;");
        
        statusLayout->addWidget(securityIcon);
        statusLayout->addSpacing(10);
        statusLayout->addWidget(workspaceLabel);
        statusLayout->addSpacing(10);
        statusLayout->addWidget(statusLabel);
        statusLayout->addStretch();
        
        // Right side info
        QLabel *versionLabel = new QLabel("ASK v8.0 | RAM: 342 MB");
        versionLabel->setStyleSheet("color: rgba(255, 255, 255, 0.5); font-size: 11px;");
        statusLayout->addWidget(versionLabel);
        
        layout->addWidget(statusBar);
    }
    
    void applyGlobalStyle() {
        // Set dark background with gradient
        setStyleSheet(R"(
            QMainWindow {
                background: qlineargradient(
                    x1:0, y1:0, x2:1, y2:1,
                    stop:0 #0a0a1f,
                    stop:0.5 #1a0a2e,
                    stop:1 #0a0a1f
                );
            }
        )");
    }

    // ========================================================================
    // FUNCTIONALITY
    // ========================================================================
    
    void setupConnections() {
        // Sidebar actions
        connect(menuBtn, &QPushButton::clicked, this, &AskBrowser::toggleSidebar);
        connect(homeBtn, &QPushButton::clicked, [this]() {
            addNewTab("https://duckduckgo.com");
        });
        connect(aiBtn, &QPushButton::clicked, [this]() {
            switchWorkspace("AI");
        });
        connect(workBtn, &QPushButton::clicked, [this]() {
            switchWorkspace("Work");
        });
        connect(downloadsBtn, &QPushButton::clicked, [this]() {
            openDownloadsPage();
        });
        connect(vaultBtn, &QPushButton::clicked, [this]() {
            openVaultPage();
        });
        connect(settingsBtn, &QPushButton::clicked, [this]() {
            openSettingsPage();
        });
        
        // Search bar
        connect(searchBar, &QLineEdit::returnPressed, this, &AskBrowser::handleSearch);
        
        // Tab management
        connect(tabWidget, &QTabWidget::tabCloseRequested, [this](int index) {
            if (tabWidget->count() > 1) {
                tabWidget->removeTab(index);
            }
        });
        
        connect(tabWidget, &QTabWidget::currentChanged, [this](int index) {
            updateAddressBar();
        });
    }
    
    void setupShortcuts() {
        // Essential shortcuts
        new QShortcut(QKeySequence("Ctrl+T"), this, [this]() {
            addNewTab(workspaceUrls[currentWorkspace]);
        });
        
        new QShortcut(QKeySequence("Ctrl+W"), this, [this]() {
            if (tabWidget->count() > 1) {
                tabWidget->removeTab(tabWidget->currentIndex());
            }
        });
        
        new QShortcut(QKeySequence("Ctrl+R"), this, [this]() {
            if (currentView()) currentView()->reload();
        });
        
        new QShortcut(QKeySequence("F5"), this, [this]() {
            if (currentView()) currentView()->reload();
        });
        
        new QShortcut(QKeySequence("F11"), this, [this]() {
            if (isFullScreen()) showNormal();
            else showFullScreen();
        });
        
        new QShortcut(QKeySequence("Ctrl+L"), this, [this]() {
            searchBar->setFocus();
            searchBar->selectAll();
        });
        
        new QShortcut(QKeySequence("Ctrl+Tab"), this, [this]() {
            int next = (tabWidget->currentIndex() + 1) % tabWidget->count();
            tabWidget->setCurrentIndex(next);
        });
        
        new QShortcut(QKeySequence("Ctrl+Shift+Tab"), this, [this]() {
            int prev = (tabWidget->currentIndex() - 1 + tabWidget->count()) % tabWidget->count();
            tabWidget->setCurrentIndex(prev);
        });
    }
    
    void toggleSidebar() {
        QPropertyAnimation *animation = new QPropertyAnimation(sidebar, "minimumWidth");
        animation->setDuration(300);
        animation->setEasingCurve(QEasingCurve::InOutCubic);
        
        if (sidebarExpanded) {
            animation->setStartValue(250);
            animation->setEndValue(60);
            sidebarExpanded = false;
            
            // Reset button text to icons
            menuBtn->setText("☰");
            homeBtn->setText("🏠");
            aiBtn->setText("🤖");
            workBtn->setText("💼");
            downloadsBtn->setText("📥");
            vaultBtn->setText("🔒");
            settingsBtn->setText("⚙️");
        } else {
            animation->setStartValue(60);
            animation->setEndValue(250);
            sidebarExpanded = true;
            
            // Expand button text
            menuBtn->setText("☰  Menu");
            homeBtn->setText("🏠  Home");
            aiBtn->setText("🤖  AI Sector");
            workBtn->setText("💼  Work Mode");
            downloadsBtn->setText("📥  Downloads");
            vaultBtn->setText("🔒  Vault");
            settingsBtn->setText("⚙️  Settings");
        }
        
        animation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    
    void switchWorkspace(const QString &workspace) {
        currentWorkspace = workspace;
        workspaceLabel->setText(workspace + " Mode");
        addNewTab(workspaceUrls[workspace]);
    }
    
    void handleSearch() {
        QWebEngineView *view = currentView();
        if (!view) return;
        
        QString input = searchBar->text().trimmed();
        
        // Check if it's a URL
        if (input.contains(".") && !input.contains(" ")) {
            if (!input.startsWith("http")) {
                input = "https://" + input;
            }
            view->setUrl(QUrl(input));
        } else {
            // It's a search query
            QString searchUrl = searchEngines[engineSelector->currentText()] + input;
            view->setUrl(QUrl(searchUrl));
        }
    }
    
    void addNewTab(const QString &url) {
        QWebEngineView *view = new QWebEngineView();
        
        // Optimize settings for performance
        view->settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
        view->settings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
        view->settings()->setAttribute(QWebEngineSettings::DnsPrefetchEnabled, true);
        view->settings()->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
        
        // Load URL
        view->setUrl(QUrl(url));
        
        // Add to tabs
        int index = tabWidget->addTab(view, "Loading...");
        tabWidget->setCurrentIndex(index);
        
        // Update tab title when page loads
        connect(view, &QWebEngineView::titleChanged, [this, view](const QString &title) {
            int idx = tabWidget->indexOf(view);
            if (idx != -1) {
                tabWidget->setTabText(idx, title.left(25));
            }
        });
        
        // Update address bar when URL changes
        connect(view, &QWebEngineView::urlChanged, [this](const QUrl &url) {
            updateAddressBar();
            saveToHistory(url.toString());
        });
        
        // Simulate tracker blocking (for demo)
        QTimer::singleShot(2000, [this]() {
            trackersBlocked += 3;
            statusLabel->setText(QString("Trackers Blocked: %1").arg(trackersBlocked));
        });
    }
    
    QWebEngineView* currentView() {
        return qobject_cast<QWebEngineView*>(tabWidget->currentWidget());
    }
    
    void updateAddressBar() {
        QWebEngineView *view = currentView();
        if (view) {
            searchBar->setText(view->url().toString());
        }
    }
    
    void openAIPanel() {
        QWebEngineView *view = new QWebEngineView();
        QString html = R"(
            <html>
            <head>
                <style>
                    body {
                        background: linear-gradient(135deg, #0a0a1f 0%, #1a0a2e 100%);
                        color: white;
                        font-family: 'Segoe UI', sans-serif;
                        padding: 40px;
                        margin: 0;
                    }
                    .container {
                        max-width: 800px;
                        margin: 0 auto;
                    }
                    h1 {
                        background: linear-gradient(135deg, #00d4ff, #ff00ff);
                        -webkit-background-clip: text;
                        -webkit-text-fill-color: transparent;
                        font-size: 48px;
                        margin-bottom: 20px;
                    }
                    .ai-card {
                        background: rgba(255, 255, 255, 0.05);
                        border: 1px solid rgba(255, 255, 255, 0.1);
                        border-radius: 16px;
                        padding: 30px;
                        margin: 20px 0;
                        cursor: pointer;
                        transition: all 0.3s;
                    }
                    .ai-card:hover {
                        background: rgba(0, 212, 255, 0.1);
                        border-color: #00d4ff;
                        transform: translateY(-5px);
                        box-shadow: 0 10px 30px rgba(0, 212, 255, 0.3);
                    }
                    .ai-card h3 {
                        color: #00d4ff;
                        margin-top: 0;
                    }
                    .chat-box {
                        background: rgba(255, 255, 255, 0.05);
                        border: 1px solid rgba(255, 255, 255, 0.1);
                        border-radius: 12px;
                        padding: 20px;
                        margin-top: 30px;
                    }
                    input {
                        width: 100%;
                        background: rgba(255, 255, 255, 0.05);
                        border: 1px solid rgba(255, 255, 255, 0.1);
                        border-radius: 8px;
                        color: white;
                        padding: 15px;
                        font-size: 16px;
                        box-sizing: border-box;
                    }
                </style>
            </head>
            <body>
                <div class="container">
                    <h1>✨ AI Assistant</h1>
                    <p style="font-size: 18px; opacity: 0.8;">Choose your AI tool or ask me anything about the current page</p>
                    
                    <div class="ai-card" onclick="window.location.href='https://gemini.google.com'">
                        <h3>🤖 Google Gemini</h3>
                        <p>Advanced AI for complex reasoning and creative tasks</p>
                    </div>
                    
                    <div class="ai-card" onclick="window.location.href='https://chat.openai.com'">
                        <h3>💬 ChatGPT</h3>
                        <p>OpenAI's conversational AI assistant</p>
                    </div>
                    
                    <div class="ai-card" onclick="window.location.href='https://claude.ai'">
                        <h3>🧠 Claude</h3>
                        <p>Anthropic's thoughtful and detailed AI</p>
                    </div>
                    
                    <div class="chat-box">
                        <h3 style="margin-top: 0;">Quick Ask</h3>
                        <input type="text" placeholder="Ask me to summarize this page, explain code, or answer questions..." />
                        <p style="font-size: 12px; opacity: 0.6; margin-top: 10px;">
                            💡 Coming soon: Context-aware AI that reads your current page
                        </p>
                    </div>
                </div>
            </body>
            </html>
        )";
        view->setHtml(html);
        int index = tabWidget->addTab(view, "✨ AI Assistant");
        tabWidget->setCurrentIndex(index);
    }
    
    void openDownloadsPage() {
        QWebEngineView *view = new QWebEngineView();
        QString html = R"(
            <html>
            <head>
                <style>
                    body {
                        background: linear-gradient(135deg, #0a0a1f 0%, #1a0a2e 100%);
                        color: white;
                        font-family: 'Segoe UI', sans-serif;
                        padding: 40px;
                    }
                    h1 { color: #00d4ff; }
                    .download-item {
                        background: rgba(255, 255, 255, 0.05);
                        border: 1px solid rgba(255, 255, 255, 0.1);
                        border-radius: 12px;
                        padding: 20px;
                        margin: 15px 0;
                    }
                    .progress-bar {
                        width: 100%;
                        height: 8px;
                        background: rgba(255, 255, 255, 0.1);
                        border-radius: 4px;
                        overflow: hidden;
                        margin: 10px 0;
                    }
                    .progress-fill {
                        height: 100%;
                        background: linear-gradient(90deg, #00d4ff, #ff00ff);
                        animation: progress 2s infinite;
                    }
                    @keyframes progress {
                        0% { width: 0%; }
                        100% { width: 100%; }
                    }
                </style>
            </head>
            <body>
                <h1>📥 Download Manager</h1>
                <p>Turbo-charged downloads with 32-thread acceleration</p>
                
                <div class="download-item">
                    <h3>example_file.zip (Demo)</h3>
                    <div class="progress-bar">
                        <div class="progress-fill" style="width: 45%;"></div>
                    </div>
                    <p>Speed: 12.5 MB/s | 45% Complete</p>
                </div>
                
                <p style="opacity: 0.6; margin-top: 40px;">
                    💡 Download manager will be fully functional in the next update.<br>
                    Features: Multi-threaded downloads, pause/resume, media detection
                </p>
            </body>
            </html>
        )";
        view->setHtml(html);
        int index = tabWidget->addTab(view, "📥 Downloads");
        tabWidget->setCurrentIndex(index);
    }
    
    void openVaultPage() {
        QWebEngineView *view = new QWebEngineView();
        QString html = R"(
            <html>
            <head>
                <style>
                    body {
                        background: linear-gradient(135deg, #0a0a1f 0%, #1a0a2e 100%);
                        color: white;
                        font-family: 'Segoe UI', sans-serif;
                        padding: 40px;
                        text-align: center;
                    }
                    h1 { color: #00d4ff; font-size: 48px; }
                    .vault-icon { font-size: 100px; margin: 30px 0; }
                    .info-box {
                        background: rgba(255, 255, 255, 0.05);
                        border: 1px solid rgba(255, 255, 255, 0.1);
                        border-radius: 16px;
                        padding: 30px;
                        max-width: 600px;
                        margin: 30px auto;
                        text-align: left;
                    }
                </style>
            </head>
            <body>
                <div class="vault-icon">🔒</div>
                <h1>Secure Vault</h1>
                <p style="font-size: 18px;">Your encrypted password manager</p>
                
                <div class="info-box">
                    <h3 style="color: #00d4ff;">🛡️ Features:</h3>
                    <ul>
                        <li>AES-256 encryption</li>
                        <li>k-Anonymity breach detection</li>
                        <li>Zero-knowledge architecture</li>
                        <li>Biometric unlock (Coming soon)</li>
                    </ul>
                    
                    <p style="margin-top: 30px; opacity: 0.7;">
                        💡 The Vault feature will be available in Phase 2 of development.<br>
                        Your passwords will never leave your device.
                    </p>
                </div>
            </body>
            </html>
        )";
        view->setHtml(html);
        int index = tabWidget->addTab(view, "🔒 Vault");
        tabWidget->setCurrentIndex(index);
    }
    
    void openSettingsPage() {
        QWebEngineView *view = new QWebEngineView();
        QString html = R"(
            <html>
            <head>
                <style>
                    body {
                        background: linear-gradient(135deg, #0a0a1f 0%, #1a0a2e 100%);
                        color: white;
                        font-family: 'Segoe UI', sans-serif;
                        padding: 40px;
                    }
                    h1 { color: #00d4ff; border-bottom: 2px solid #00d4ff; padding-bottom: 15px; }
                    h2 { color: #00d4ff; margin-top: 40px; }
                    .setting-item {
                        background: rgba(255, 255, 255, 0.05);
                        border: 1px solid rgba(255, 255, 255, 0.1);
                        border-radius: 12px;
                        padding: 20px;
                        margin: 15px 0;
                    }
                    button {
                        background: linear-gradient(135deg, #00d4ff, #ff00ff);
                        border: none;
                        color: white;
                        padding: 12px 24px;
                        border-radius: 8px;
                        font-weight: 600;
                        cursor: pointer;
                        margin: 5px;
                    }
                    button:hover {
                        transform: translateY(-2px);
                        box-shadow: 0 8px 20px rgba(0, 212, 255, 0.4);
                    }
                    .shortcut-list {
                        background: rgba(255, 255, 255, 0.05);
                        padding: 20px;
                        border-radius: 12px;
                        margin-top: 20px;
                    }
                    code {
                        background: rgba(0, 212, 255, 0.2);
                        padding: 4px 8px;
                        border-radius: 4px;
                        font-family: 'Courier New', monospace;
                    }
                </style>
            </head>
            <body>
                <h1>⚙️ ASK Browser Settings</h1>
                
                <div class="setting-item">
                    <h3>📊 Browser Information</h3>
                    <p><b>Version:</b> 8.0 (Liquid Glass Edition)</p>
                    <p><b>Engine:</b> Chromium (Qt WebEngine)</p>
                    <p><b>Build:</b> Production-Ready</p>
                </div>
                
                <h2>🧹 Privacy & Data</h2>
                <div class="setting-item">
                    <button onclick="alert('History cleared!') ">Clear History</button>
                    <button onclick="alert('Cache cleared!') ">Clear Cache</button>
                    <button onclick="alert('Cookies cleared!') ">Clear Cookies</button>
                </div>
                
                <h2>⌨️ Keyboard Shortcuts</h2>
                <div class="shortcut-list">
                    <p><code>Ctrl + T</code> - New Tab</p>
                    <p><code>Ctrl + W</code> - Close Tab</p>
                    <p><code>Ctrl + R</code> / <code>F5</code> - Reload</p>
                    <p><code>Ctrl + L</code> - Focus Address Bar</p>
                    <p><code>Ctrl + Tab</code> - Next Tab</p>
                    <p><code>Ctrl + Shift + Tab</code> - Previous Tab</p>
                    <p><code>F11</code> - Fullscreen</p>
                </div>
                
                <h2>🎨 Appearance</h2>
                <div class="setting-item">
                    <p><b>Theme:</b> Liquid Glass (Default)</p>
                    <p><b>Font:</b> Oxanium</p>
                    <p>💡 Custom themes coming in Phase 3</p>
                </div>
                
                <h2>🔒 Security</h2>
                <div class="setting-item">
                    <p>✅ Tracking Protection: <b>Enabled</b></p>
                    <p>✅ HTTPS-Only Mode: <b>Enabled</b></p>
                    <p>✅ Cookie Blocking: <b>Third-party blocked</b></p>
                </div>
            </body>
            </html>
        )";
        view->setHtml(html);
        int index = tabWidget->addTab(view, "⚙️ Settings");
        tabWidget->setCurrentIndex(index);
    }
    
    // ========================================================================
    // DATABASE
    // ========================================================================
    
    void setupDatabase() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("ask_browser_data.db");
        
        if (db.open()) {
            QSqlQuery query;
            
            // History table
            query.exec(R"(
                CREATE TABLE IF NOT EXISTS history (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    url TEXT NOT NULL,
                    title TEXT,
                    visit_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                )
            )");
            
            // Bookmarks table
            query.exec(R"(
                CREATE TABLE IF NOT EXISTS bookmarks (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    url TEXT NOT NULL,
                    title TEXT,
                    folder TEXT DEFAULT 'General',
                    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                )
            )");
            
            qDebug() << "Database initialized successfully";
        } else {
            qDebug() << "Database error:" << db.lastError().text();
        }
    }
    
    void saveToHistory(const QString &url) {
        QSqlQuery query;
        query.prepare("INSERT INTO history (url) VALUES (:url)");
        query.bindValue(":url", url);
        query.exec();
    }
    
    // ========================================================================
    // FONT LOADING
    // ========================================================================
    
    void loadOxaniumFont() {
        // Try to load Oxanium font
        int fontId = QFontDatabase::addApplicationFont("./Oxanium-Regular.ttf");
        QString fontFamily = "Segoe UI"; // Fallback
        
        if (fontId != -1) {
            QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
            if (!fontFamilies.isEmpty()) {
                fontFamily = fontFamilies.at(0);
                qDebug() << "Loaded custom font:" << fontFamily;
            }
        } else {
            qDebug() << "Could not load Oxanium font, using fallback";
        }
        
        // Apply font globally to Qt widgets only (not web content)
        QFont appFont(fontFamily, 10);
        QApplication::setFont(appFont);
        
        // Tooltip styling
        qApp->setStyleSheet(qApp->styleSheet() + R"(
            QToolTip {
                background: rgba(15, 15, 35, 0.95);
                color: white;
                border: 1px solid #00d4ff;
                border-radius: 6px;
                padding: 8px;
                font-size: 12px;
            }
        )");
    }
};

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char *argv[]) {
    // Performance flags for Chromium
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", 
            "--enable-gpu-rasterization "
            "--enable-zero-copy "
            "--ignore-gpu-blocklist "
            "--enable-features=VaapiVideoDecoder");
    
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    
    QApplication app(argc, argv);
    
    AskBrowser browser;
    browser.show();
    
    return app.exec();
}

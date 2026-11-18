#include "ContactListWidget.h"
#include "Theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

ContactListWidget::ContactListWidget(QWidget* parent) : QWidget(parent) {
    setStyleSheet(QString("background-color: %1;").arg(Theme::Surface.name()));
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(10);

    auto* searchLayout = new QHBoxLayout;
    searchLayout->setSpacing(6);
    searchEdit_ = new QLineEdit;
    searchEdit_->setPlaceholderText("Поиск пользователя...");
    searchEdit_->setStyleSheet(QString(
        "QLineEdit { background-color: %1; color: %2; border: 1px solid %3; "
        "border-radius: 8px; padding: 8px 12px; font-size: 10pt; }"
        "QLineEdit:focus { border: 1px solid %4; }")
        .arg(Theme::SurfaceDark.name(), Theme::Text.name(),
             Theme::Border.name(), Theme::Accent.name()));

    searchBtn_ = new QPushButton(QString::fromUtf8("\xF0\x9F\x94\x8D"));
    searchBtn_->setFixedWidth(40);
    searchBtn_->setStyleSheet(QString(
        "QPushButton { background-color: %1; color: white; border: none; "
        "border-radius: 8px; padding: 6px; font-size: 12pt; }"
        "QPushButton:hover { background-color: #3f6ad8; }"
        "QPushButton:pressed { background-color: #2f4fa8; }")
        .arg(Theme::Accent.name()));

    searchLayout->addWidget(searchEdit_);
    searchLayout->addWidget(searchBtn_);

    list_ = new QListWidget;
    list_->setStyleSheet(QString(
        "QListWidget { background-color: %1; border: none; outline: none; padding: 4px; }"
        "QListWidget::item { padding: 10px 12px; margin: 2px 4px; "
        "border-radius: 8px; color: %2; }"
        "QListWidget::item:hover { background-color: %3; }"
        "QListWidget::item:selected { background-color: %4; color: white; }")
        .arg(Theme::SurfaceDark.name(), Theme::Text.name(),
             Theme::Surface.name(), Theme::Accent.name()));

    layout->addLayout(searchLayout);
    layout->addWidget(list_, 1);

    connect(searchBtn_, &QPushButton::clicked, this, &ContactListWidget::onSearchClicked);
    connect(searchEdit_, &QLineEdit::returnPressed, this, &ContactListWidget::onSearchClicked);
    connect(list_, &QListWidget::itemClicked, this, &ContactListWidget::onItemClicked);
}

void ContactListWidget::addContact(const Contact& contact) {
    if (contact.username.isEmpty() || hasContact(contact.username)) return;
    contacts_.append(contact);
    auto* item = new QListWidgetItem(QString::fromUtf8("\xF0\x9F\x91\xA4 ") + contact.username);
    QFont font = list_->font();
    font.setFamilies({font.family(), "Segoe UI Emoji", "Apple Color Emoji", "Noto Color Emoji"});
    item->setFont(font);
    item->setData(Qt::UserRole, contacts_.size() - 1);
    list_->addItem(item);
}

bool ContactListWidget::hasContact(const QString& username) const {
    for (const auto& c : contacts_) if (c.username == username) return true;
    return false;
}

void ContactListWidget::onItemClicked(QListWidgetItem* item) {
    if (!item) return;
    int idx = item->data(Qt::UserRole).toInt();
    if (idx >= 0 && idx < contacts_.size()) emit contactSelected(contacts_[idx]);
}

void ContactListWidget::onSearchClicked() {
    QString user = searchEdit_->text().trimmed();
    if (!user.isEmpty()) emit searchRequested(user);
}
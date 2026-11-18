#include "ContactListWidget.h"
#include "Theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

ContactListWidget::ContactListWidget(QWidget* parent) : QWidget(parent) {
    setStyleSheet(QString("background-color: %1;").arg(Theme::Surface.name()));
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(8);

    auto* searchLayout = new QHBoxLayout;
    searchEdit_ = new QLineEdit;
    searchEdit_->setPlaceholderText("Поиск пользователя...");
    searchEdit_->setStyleSheet(QString(
        "QLineEdit { background-color: %1; color: %2; border: 1px solid %3; border-radius: 6px; padding: 6px; }")
        .arg(Theme::SurfaceDark.name(), Theme::Text.name(), Theme::Border.name()));

    searchBtn_ = new QPushButton("🔍");
    searchBtn_->setFixedWidth(40);
    searchBtn_->setStyleSheet(QString(
        "QPushButton { background-color: %1; color: white; border: none; border-radius: 6px; padding: 6px; }")
        .arg(Theme::Accent.name()));

    searchLayout->addWidget(searchEdit_);
    searchLayout->addWidget(searchBtn_);

    list_ = new QListWidget;
    list_->setStyleSheet(QString(
        "QListWidget { background-color: %1; border: none; outline: none; }"
        "QListWidget::item { padding: 10px; color: %2; border-radius: 4px; }"
        "QListWidget::item:selected { background-color: %3; color: white; }")
        .arg(Theme::SurfaceDark.name(), Theme::Text.name(), Theme::Accent.name()));

    layout->addLayout(searchLayout);
    layout->addWidget(list_, 1);

    connect(searchBtn_, &QPushButton::clicked, this, &ContactListWidget::onSearchClicked);
    connect(searchEdit_, &QLineEdit::returnPressed, this, &ContactListWidget::onSearchClicked);
    connect(list_, &QListWidget::itemClicked, this, &ContactListWidget::onItemClicked);
}

void ContactListWidget::addContact(const Contact& contact) {
    if (contact.username.isEmpty() || hasContact(contact.username)) return;
    contacts_.append(contact);
    auto* item = new QListWidgetItem("👤 " + contact.username);
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

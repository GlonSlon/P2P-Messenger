#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

struct Contact {
    QString username;
};

class ContactListWidget : public QWidget {
    Q_OBJECT
public:
    explicit ContactListWidget(QWidget* parent = nullptr);
    void addContact(const Contact& contact);
    bool hasContact(const QString& username) const;

signals:
    void contactSelected(const Contact& contact);
    void searchRequested(const QString& username);

private slots:
    void onItemClicked(QListWidgetItem* item);
    void onSearchClicked();

private:
    QLineEdit*   searchEdit_;
    QPushButton* searchBtn_;
    QListWidget* list_;
    QList<Contact> contacts_;
};
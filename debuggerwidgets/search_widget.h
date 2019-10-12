#ifndef SEARCH_WIDGET_H
#define SEARCH_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

class SearchWidget : public QWidget
{
    Q_OBJECT
public:
    SearchWidget();
    void setSearchVisible(bool);
    void setGoToVisible(bool);
signals:
    void search(QString);
    void goTo(uint32_t);
private:
    QPushButton *searchButton = nullptr;
    QPushButton *goToButton = nullptr;
    QLineEdit *searchQuery = nullptr;
private slots:
    void searchClicked();
    void goToClicked();
};

#endif

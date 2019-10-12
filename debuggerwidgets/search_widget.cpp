#include "search_widget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>

SearchWidget::SearchWidget()
{
    QHBoxLayout* searchLayout = new QHBoxLayout(this);
    QLabel* searchLabel = new QLabel("Search");
    searchLayout->addWidget(searchLabel);
    searchQuery = new QLineEdit();
    searchLayout->addWidget(searchQuery);
    searchButton = new QPushButton("Search");
    searchButton->setVisible(false);
    searchLayout->addWidget(searchButton);
    goToButton = new QPushButton("Go To Address");
    goToButton->setVisible(false);
    searchLayout->addWidget(goToButton);
    connect(searchButton, SIGNAL(clicked()), this, SLOT(searchClicked()));
    connect(goToButton, SIGNAL(clicked()), this, SLOT(goToClicked()));
}

void SearchWidget::setSearchVisible(bool visible)
{
    searchButton->setVisible(visible);
}

void SearchWidget::setGoToVisible(bool visible)
{
    goToButton->setVisible(visible);
}

void SearchWidget::searchClicked()
{
    emit search(searchQuery->text());
}

void SearchWidget::goToClicked()
{
    QString qs_address = searchQuery->text();
    uint32_t address = qs_address.toUInt(nullptr, 16);
    emit goTo(address);
}

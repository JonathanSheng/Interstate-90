void add_order(Book& book, AddOrder& order){
    Order bookOrder;
    bookOrder.stock_locate = order.stock_locate;
    bookOrder.side = order.side;
    bookOrder.price = order.price;
    bookOrder.shares = order.shares;
    
    book.orders[order.order_reference] = bookOrder;
}

void execute_order();

void cancel_order();

void delete_order();

void replace_order();
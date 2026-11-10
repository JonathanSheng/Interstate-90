
//Bare minimum of tracking an order
struct Order {
    uint16_t stock_locate; //Stock locate number
    Side side; //Sell or Buy
    uint32_t price; //price
    uint32_t shares; //shares

};

//Book holds a mapping of every order_reference: order info
struct Book {
    std::unordered_map<uint64_t, Order> orders; // Unique order reference number: order info
};
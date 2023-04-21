#pragma  once

class Equipment {
private:
    int size;
    std::vector<std::shared_ptr<class Product>> equipment;
    eventpp::CallbackList<void()> equipmentChanged;

public:
    bool AddProduct(const std::shared_ptr<Product>& product);
    bool RequestProduct(const std::shared_ptr<Product>& product);
    bool CheckProduct(const std::shared_ptr<Product>& product) const;
    bool CheckBlueprint(const class Blueprint& blueprint) const;
    bool CheckIsFull() const;
    ~Equipment() = default;

};


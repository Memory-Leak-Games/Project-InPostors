#pragma once

#include <vector>
#include "Product.h"

class Blueprint {
private:
    std::vector<Product> input;
    Product output;
    int processingTime;
public:
    const std::vector<Product>& GetInput() const;
    void SetInput(const std::vector<Product>& input);
    const Product& GetOutput() const;
    void SetOutput(const Product& output);
    int GetProcessingTime() const;
    void SetProcessingTime(int processingTime);

};

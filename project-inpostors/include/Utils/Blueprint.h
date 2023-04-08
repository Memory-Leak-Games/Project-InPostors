#pragma once

#include <vector>
#include "Product.h"

class Blueprint {
private:
    std::vector<std::shared_ptr<Product>> input;
    std::shared_ptr<Product> output;
    int processingTime;

public:
    const std::vector<std::shared_ptr<Product>>& GetInput() const;
    void SetInput(const std::vector<std::shared_ptr<Product>>& input);
    const std::shared_ptr<Product>& GetOutput() const;
    void SetOutput(const std::shared_ptr<Product>& output);
    int GetProcessingTime() const;
    void SetProcessingTime(int processingTime);


};

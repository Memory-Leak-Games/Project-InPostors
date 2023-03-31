#include "include/Utils/Blueprint.h"

const std::vector<Product>& Blueprint::GetInput() const {
    return input;
}

void Blueprint::SetInput(const std::vector<Product>& input) {
    Blueprint::input = input;
}

const Product& Blueprint::GetOutput() const {
    return output;
}

void Blueprint::SetOutput(const Product& output) {
    Blueprint::output = output;
}

int Blueprint::GetProcessingTime() const {
    return processingTime;
}

void Blueprint::SetProcessingTime(int processingTime) {
    Blueprint::processingTime = processingTime;
}

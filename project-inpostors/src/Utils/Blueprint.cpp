#include "include/Utils/Blueprint.h"

const std::vector<std::shared_ptr<Product>>& Blueprint::GetInput() const {
    return input;
}

void Blueprint::SetInput(const std::vector<std::shared_ptr<Product>>& input) {
    Blueprint::input = input;
}

const std::shared_ptr<Product>& Blueprint::GetOutput() const {
    return output;
}

void Blueprint::SetOutput(const std::shared_ptr<Product>& output) {
    Blueprint::output = output;
}

int Blueprint::GetProcessingTime() const {
    return processingTime;
}

void Blueprint::SetProcessingTime(int processingTime) {
    Blueprint::processingTime = processingTime;
}

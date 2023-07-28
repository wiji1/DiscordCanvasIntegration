#include <iostream>
#include <vector>
#include <memory>


class Test {
private:
    int data;
public:
    Test() : data{0} { std::cout << "Test Constructor" << std::endl; }
    explicit Test(int data) : data{data} { std::cout << "Test Constructor" << std::endl; }

    ~Test() { std::cout << "Test Destructor" << std::endl; }

    [[nodiscard]] int get_data() const { return data; }
};

std::unique_ptr<std::vector<std::shared_ptr<Test>>> make();
void fill(std::vector<std::shared_ptr<Test>> &vec, int num);
void display(std::vector<std::shared_ptr<Test>> &vec);


int main() {
    std::unique_ptr<std::vector<std::shared_ptr<Test>>> vec_ptr;
    vec_ptr = make();

    std::cout << "Please enter the amount of data points you would like to enter:" << std::endl;
    int num {};
    std::cin >> num;
    fill(*vec_ptr, num);
    display(*vec_ptr);

    return 0;
}

std::unique_ptr<std::vector<std::shared_ptr<Test>>> make() {
    std::unique_ptr ptr {std::make_unique<std::vector<std::shared_ptr<Test>>>()};

    return ptr;
}

void fill(std::vector<std::shared_ptr<Test>> &vec, int num) {
    for(int i{}; i < num; i++) {
        std::cout << "What number would you like to enter?" << std::endl;

        int temp_num {};
        std::cin >> temp_num;

        vec.push_back(std::make_shared<Test>(temp_num));
    }
}

void display(std::vector<std::shared_ptr<Test>> &vec) {
    std::cout << "==================[Data]==================" << std::endl;
    for(const auto &ptr: vec) {
        std::cout << "Value: " << ptr->get_data() << std::endl;
    }
    std::cout << "==========================================" << std::endl;
}


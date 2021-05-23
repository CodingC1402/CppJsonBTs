// JsonBTs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <chrono>
#include "BTs.h"

class PrintNode : public LeafNode
{
public:
    void CustomLoad(nlohmann::json& input) override {
        _output = input["String"].get<std::string>();
    }
    Node::State Tick() override {
        std::cout << _output << std::endl;
        return Node::State::Success;
    }
    LEAF_DEFAULT_CLONE(PrintNode);
protected:
    std::string _output;
private:
    NODE_REGISTER(PrintNode);
};

class SleepNode : public LeafNode
{
public:
    void CustomLoad(nlohmann::json& input) override {
        _duration = static_cast<std::chrono::duration<float>>(input["Duration"].get<float>());
    }
    Node::State Tick() override {
        std::cout << "Sleeping for " << _duration << std::endl;
        std::this_thread::sleep_for(_duration);
        std::cout << "Awake" << std::endl;
        return Node::State::Success;
    }
    LEAF_DEFAULT_CLONE(SleepNode);
protected:
    std::chrono::duration<float> _duration;
private:
    NODE_REGISTER(SleepNode);
};

class FailAt : public LeafNode
{
public:
    void CustomLoad(nlohmann::json& input) override {
        loop = input["Number"].get<int>();
        loopTime = 0;
    }
    Node::State Tick() override {
        std::this_thread::sleep_for(static_cast<std::chrono::duration<float>>(1));
        std::cout << loopTime << std::endl;
        loopTime++;

        if (loopTime == loop)
        {
            loopTime = 0;
            return Node::State::Failure;
        }
        return Node::State::Running;
    }
    LEAF_DEFAULT_CLONE(FailAt);
protected:
    int loopTime;
    int loop;
private:
    NODE_REGISTER(FailAt);
};

int main()
{
    auto tree = BTs::Load("bt.json");
    auto treeClone = tree->Clone();
    std::cout << "Hello World!\n";
    std::string input = "";
    while (input != "quit")
    {
        switch (treeClone->Tick())
        {
        case Node::State::Failure:
            std::cout << "Failure\n";
            break;
        case Node::State::Success:
            std::cout << "Success\n";
            break;
        case Node::State::Running:
            std::cout << "Running\n";
            break;
        }
        std::cin >> input;
    }
}
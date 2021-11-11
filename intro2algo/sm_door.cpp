#include <iostream>
#include <tuple>
#include <variant>
#include <functional>

using namespace std;

class OpenEvent {};
class CloseEvent {};

class ClosedState;
class OpenState;

// 定义状态
class ClosedState
{
public:
	// 该状态下囊括的转移
	OpenState* handle(const OpenEvent& e)
	{
		cout << "Current State: ClosedState \t" << endl;
		cout << "Received OpenEvent, so opening the door ..." << endl;
		return NULL;
	}

	ClosedState* handle(const CloseEvent& e)
	{
		cout << "Current State: ClosedState \t" << endl;
		cout << "Received CloseEvent, so do nothing, since the door is already closed!" << endl;
		return NULL;
	}
};

class OpenState
{
public:
	OpenState* handle(const OpenEvent& e)
	{
		cout << "Current State: OpenState \t" << endl;
		cout << "Received OpenEvent, so do nothing, since the door is already open!" << endl;
		return NULL;
	}

	ClosedState* handle(const CloseEvent&)
	{
		cout << "Current State: OpenState \t" << endl;
		cout << "Received CloseEvent, so closing the door ..." << endl;
		return NULL;
	}
};


template <typename State1, typename State2, typename ... States>
class StateMachine
{
public:
	StateMachine() : currentStatePtr(&std::get<0>(states)) {}

	template <typename Event>
	bool handle(const Event& event)
	{
		auto passEventToState = [&event](auto statePtr) {
			return statePtr->handle(event);
		};
		auto p = std::visit(passEventToState, currentStatePtr);
		currentStatePtr = &std::get<std::remove_pointer<decltype(p)>::type>(states);
		return true;
	}

private:
	std::tuple<State1, State2, States ...> states;
	std::variant<State1*, State2*, States* ...> currentStatePtr;
};

using Door = StateMachine<ClosedState, OpenState>;

void sm_door_test()
{
	Door door;

	door.handle(OpenEvent{});
	door.handle(OpenEvent{});
	door.handle(CloseEvent{});

	door.handle(CloseEvent{});
	door.handle(OpenEvent());
}

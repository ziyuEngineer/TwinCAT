#pragma once
#include <tinyfsm.hpp>
#include "FiveAxisController.h"

struct Cycle_Update : tinyfsm::Event {};

struct SystemSafetyCheck : tinyfsm::Event {};

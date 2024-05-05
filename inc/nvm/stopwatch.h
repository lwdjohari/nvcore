/*
 *  Copyright (c) 2020 Linggawasistha Djohari
 * <linggawasistha.djohari@outlook.com> Licensed to Linggawasistha Djohari under
 * one or more contributor license agreements. See the NOTICE file distributed
 * with this work for additional information regarding copyright ownership.
 *
 *  Linggawasistha Djohari licenses this file to you under the Apache License,
 *  Version 2.0 (the "License"); you may not use this file except in
 *  compliance with the License. You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef NVM_CORE_V2_STOPWATCH_H
#define NVM_CORE_V2_STOPWATCH_H

#include <chrono>

namespace nvm{

class Stopwatch {
public:
    Stopwatch() : start(std::chrono::high_resolution_clock::now()), running(true) {}

    // Reset the timer
    void Reset() {
        start = std::chrono::high_resolution_clock::now();
        running = true;
    }

    // Get the elapsed time in milliseconds
    double ElapsedMilliseconds() const {
        if (!running) {
            throw std::runtime_error("Timer is not running. Reset the timer to start it.");
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return static_cast<double>(elapsed.count());
    }

    // Stop the stopwatch
    void Stop() {
        if (!running) {
            throw std::runtime_error("Timer is already stopped.");
        }
        end = std::chrono::high_resolution_clock::now();
        running = false;
    }

    // Check if the stopwatch is running
    bool IsRunning() const {
        return running;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
    bool running;
};
}
#endif

// ========================================================================================================
// File: MockStateStorage.hpp
// Purpose: In-memory simulation of the static Flash checkpoint area for M4.2 unit tests.
// ========================================================================================================

#ifndef MOCK_STATE_STORAGE_HPP
#define MOCK_STATE_STORAGE_HPP

#include "storage/StorageInterfaces.hpp"

namespace StepMaster {
namespace Storage {
namespace Test {

    class MockStateStorage : public IStateStorage {
    public:
        PersistentState memory{};
        bool isFormatted = false;

        bool loadState(PersistentState& outState) override {
            if (!isFormatted) return false;
            outState = memory;
            return true;
        }

        bool saveState(const PersistentState& state) override {
            memory = state;
            isFormatted = true;
            return true;
        }
    };

} // namespace Test
} // namespace Storage
} // namespace StepMaster

#endif // MOCK_STATE_STORAGE_HPP

1. Edit true-function.h and add a new item to enum FunctionId
2. See if the function belongs to an existing class like finite-scalar-functions etc. and, if so, add its specific implementation there.
3. Otherwise, add a new cpp file implementing semantics.
    1. #include "true-function-creation.h" 
    2. Define a class derived from TrueFunctionNodeBase to implement function semantics
    2. Use SPR_MAP_FUNCTION_ID_TO_IMPL to connect it to function id and define its data

Function template

```cpp
#include "true-function-creation.h"
#include "../execution-context.h"

namespace Spreader {

    class FunctionXxx : public TrueFunctionNodeBase {
        using Super = TrueFunctionNodeBase;

    protected:
        struct ExecutionStackEntry : Super::ExecutionStackEntry {
            using Super::ExecutionStackEntry::ExecutionStackEntry;

            //Add any data function need on local "stack" here
            //If no data remove this struct entirely - base one will do
        };

        FunctionXxx(ArgumentList && args) noexcept:
            Super(std::move(args)) {
        }
        ~FunctionXxx() noexcept = default;
    public:

        // Uncomment if you need to do something before any arguments are processed
        // void onBeforeArguments(ExecutionContext & context) const override {
        //     auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
        //     
        // }

        // Uncomment if function has arguments
        // auto onAfterArgument(ExecutionContext & context) const -> TraversalEventOutcome override {
        //     auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);
        // }

        //Return the evaluation result in context.returnedValue/context.returnedExtent
        auto execute(ExecutionContext & context) const -> bool override {

            auto entry = static_cast<ExecutionStackEntry *>(context.stackPointer);

            if (!context.suppressEvaluation) {
                context.returnedValue = ...;
            }
            context.returnedExtent = ...;
            return true;
        }

    };
    
    SPR_MAP_FUNCTION_ID_TO_IMPL(FunctionId::Xxx, "NAME_IN_SHEET", MIN_ARGS, MAX_ARGS, FunctionXxx);

}


```
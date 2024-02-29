//EXAMPLE: Favor Straight-Line Code
// Before: Using if-else
int getSpeedModifier(bool isRainy) {
    if (isRainy) {
        return 2;
    } else {
        return 1;
    }
}

// After: Using conditional operator
int getSpeedModifier(bool isRainy) {
    return isRainy ? 2 : 1;
}



//EXAMPLE: Optimize Loop Termination Conditions
// Predictable loop termination
for (int i = 0; i < 100; ++i) {
    // Loop body
}


//EXAMPLE: Reorder Branches Based on Likelihood
// Before: Less likely condition first
if (rareCondition) {
    handleRareCondition();
} else {
    handleCommonCase();
}

// After: Most likely condition first
if (commonCase) {
    handleCommonCase();
} else {
    handleRareCondition();
}


//EXAMPLE: Use Compiler Hints
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

if (likely(commonCase)) {
    handleCommonCase();
} else {
    handleRareCondition();
}



//EXAMPLE: Simplify Complex Conditions
// Before: Complex condition
if (isHoliday && numberOfVisitors > 500) {
    // Special handling
}

// After: Simplified conditions
bool isBusyHoliday = isHoliday && (numberOfVisitors > 500);
if (isBusyHoliday) {
    // Special handling
}

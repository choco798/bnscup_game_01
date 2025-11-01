# Grade Selection System Test Plan

## Functionality Tests

### 1. Grade Selection UI
- [ ] Grade buttons display correctly on title screen
- [ ] Each grade shows correct unlocked/locked status
- [ ] Selected grade is visually highlighted
- [ ] Cannot select locked grades
- [ ] Can select any unlocked grade

### 2. Grade Availability
- [ ] New player starts with only Grade 0 unlocked
- [ ] After promotion, previous and current grades are available
- [ ] Cannot access grades higher than current rank
- [ ] Grade availability persists after game restart

### 3. Problem Distribution
- [ ] Problems are filtered correctly by selected grade
- [ ] All problems in selected grade are available
- [ ] Problems are properly shuffled
- [ ] No problems from other grades appear
- [ ] Empty grade handling (show appropriate message)

### 4. Grade Progression
- [ ] Completing problems in current grade contributes to promotion
- [ ] Lower grade completions don't affect promotion
- [ ] Promotion requirements are correctly checked
- [ ] Grade progress is saved properly

### 5. UI Feedback
- [ ] Clear indication of selected grade
- [ ] Appropriate feedback when selecting locked grade
- [ ] Visual feedback for grade availability
- [ ] Error messages for invalid selections

## Test Scenarios

### Scenario 1: New Player
1. Start new game
2. Verify only Grade 0 is available
3. Try selecting locked grades
4. Complete Grade 0 problems
5. Verify promotion mechanics

### Scenario 2: Mid-rank Player
1. Load save with Rank 1 (名人)
2. Verify Grades 0-1 are available
3. Verify Grade 2 is locked
4. Play problems from different grades
5. Check progress tracking

### Scenario 3: Grade Selection
1. Select available grade
2. Verify problem set matches grade
3. Complete session
4. Check progress updates
5. Verify persistence of progress

### Scenario 4: Problem Distribution
1. Select specific grade
2. Play multiple sessions
3. Verify problem variety
4. Check problem shuffling
5. Verify grade-specific problems only

## Known Issues
(To be filled during testing)

## Test Environment
- Windows 11
- Debug build
- Clean save data for new player tests
- Prepared save data for different ranks